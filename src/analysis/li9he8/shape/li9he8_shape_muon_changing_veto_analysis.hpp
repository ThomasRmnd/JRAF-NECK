#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONCHANGINGVETOANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONCHANGINGVETOANALYSIS_HPP_

#include "analysis/li9he8/shape/li9he8_shape_muon_analysis.hpp"
#include "selection/constants.hpp"

class li9he8_shape_muon_changing_veto_analysis : public li9he8_shape_muon_analysis {

public:

    using li9he8_shape_muon_analysis::li9he8_shape_muon_analysis;

    ~li9he8_shape_muon_changing_veto_analysis() override = default;

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{0, 5000}, timestamp{0, 1000000}};

        if (!g_prompt_energy_cut.is_in(prompt)) return false;
        if (!g_delayed_hydrogen_energy_cut.is_in(delayed) && !g_delayed_carbon_energy_cut.is_in(delayed)) return false;
        if (!g_fiducial_volume_cut.is_in(prompt)) return false;
        if (!g_acrylic_sphere_cut.is_in(delayed)) return false;
        if (g_chimney_cut.is_in(prompt)) return false;
        if (!vertex_correlation_cut.is_in(delayed)) return false;
        if (!g_flasher_cut.is_in(prompt)) return false;

        std::size_t nb_multu_veto = 0ul;
        for (const vertex& multiplicity : m_nav->multiplicities) {
            if (multiplicity.ts == prompt.ts || multiplicity.ts == delayed.ts) continue;
            vertex mult{multiplicity};
            mult.e /= m_gtc.interpolate(mult.ts);
            if (!g_multiplicity_energy_cut.is_in(mult)) continue;
            if (!g_acrylic_sphere_cut.is_in(mult)) continue;
            if (mult.ts < prompt.ts - timestamp{0, 1000000} || delayed.ts + timestamp{0, 1000000} < mult.ts) continue;
            ++nb_multu_veto;
        }
        if (nb_multu_veto) return false;

        m_dlat_mu2p.clear();
        m_dlat_mu2d.clear();
        m_dt_mu2p.clear();
        m_dt_mu2d.clear();
        m_is_sig.clear();

        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        double min_dlat_mu2p = std::numeric_limits<double>::infinity();
        double min_dlat_mu2d = std::numeric_limits<double>::infinity();
        timestamp min_dt_mu2p{0, 0};
        timestamp min_dt_mu2d{0, 0};
        bool min_is_in_sig = false;
        bool is_set_min_ts_mu2p = false;

        for (const track& muon : m_nav->muons) {
            if (muon.method != m_recname) continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;

            // bool is_in_bkg = (
            //     muon.ts + m_ts_bkg_low <= prompt.ts && prompt.ts <= muon.ts + m_ts_bkg_high &&
            //     muon.ts + m_ts_bkg_low <= delayed.ts && delayed.ts <= muon.ts + m_ts_bkg_high
            // );
            bool is_in_sig = (
                muon.ts + m_ts_sig_low <= prompt.ts && prompt.ts <= muon.ts + m_ts_sig_high &&
                muon.ts + m_ts_sig_low <= delayed.ts && delayed.ts <= muon.ts + m_ts_sig_high
            );
            
            vec3 dir = unit(muon.fpos - muon.ipos);
            double d_mu2p = mag(cross(dir, prompt.pos - muon.ipos));
            double d_mu2d = mag(cross(dir, delayed.pos - muon.ipos));
            double radius_sig = 0.0;
            // double radius_bkg = 0.0;

            if (is_in_sig) {
                radius_sig = m_radius + m_radius / timestamp_to_double(m_ts_sig_low - m_ts_sig_high) * timestamp_to_double(prompt.ts - muon.ts);
            }
            // if (is_in_bkg) {
            //     radius_bkg = m_radius + m_radius / timestamp_to_double(m_ts_bkg_high - m_ts_bkg_low) * timestamp_to_double(prompt.ts - muon.ts);
            // }

            if (radius_sig < d_mu2p) continue;
            // if (radius_sig < d_mu2p && radius_bkg < d_mu2d) continue;

            if (min_dlat_mu2p < d_mu2p) continue;
            min_dlat_mu2p = d_mu2p;
            min_dlat_mu2d = d_mu2d;
            min_dt_mu2p = prompt.ts - muon.ts;
            min_dt_mu2d = delayed.ts - muon.ts;
            min_is_in_sig = is_in_sig;
            is_set_min_ts_mu2p = true;
        }

        if (is_set_min_ts_mu2p) {
            m_dlat_mu2p.push_back(min_dlat_mu2p);
            m_dlat_mu2d.push_back(min_dlat_mu2d);
            m_dt_mu2p.push_back(timestamp_to_double(min_dt_mu2p));
            m_dt_mu2d.push_back(timestamp_to_double(min_dt_mu2d));
            m_is_sig.push_back(min_is_in_sig);
        }

        return !m_is_sig.empty();
    }

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONCHANGINGVETOANALYSIS_HPP_