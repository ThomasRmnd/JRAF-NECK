#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_

#include "analysis/li9he8/shape/li9he8_shape_muon_analysis.hpp"
#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/spatial.hpp"
#include "selection/vertex.hpp"

class li9he8_shape_muon_with_neutron_analysis : public li9he8_shape_muon_analysis {

public:

    using li9he8_shape_muon_analysis::li9he8_shape_muon_analysis;

    ~li9he8_shape_muon_with_neutron_analysis() override = default;

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{0, 5000}, timestamp{0, 1000000}};

        if (!c_prompt_energy_cut.is_in(prompt)) return false;
        if (!c_delayed_energy_cut.is_in(delayed)) return false;
        if (!c_fiducial_volume_cut.is_in(prompt)) return false;
        if (c_chimney_cut.is_in(prompt)) return false;
        if (!vertex_correlation_cut.is_in(delayed)) return false;
        if (!c_flasher_cut.is_in(prompt)) return false;

        std::size_t nb_multu_veto = 0ul;
        for (const vertex& multiplicity : m_nav->multiplicities) {
            if (multiplicity.ts == prompt.ts || multiplicity.ts == delayed.ts) continue;
            vertex mult{multiplicity};
            mult.e /= m_gtc.interpolate(mult.ts);
            if (!c_multiplicity_energy_cut.is_in(mult)) continue;
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

        // stopping_muon_lookup has_stopping_in_cd_event;
        // stopping_muon_lookup has_stopping_in_wp_event;
        // has_stopping_in_cd_event.fill(m_nav, "CdClassify");
        // has_stopping_in_wp_event.fill(m_nav, "WpBasic");

        double min_dlat_mu2p = std::numeric_limits<double>::infinity();
        double min_dlat_mu2d = std::numeric_limits<double>::infinity();
        timestamp min_dt_mu2p{0, 0};
        timestamp min_dt_mu2d{0, 0};
        bool min_is_in_sig = false;
        bool is_set_min_ts_mu2p = false;

        for (const track& muon : m_nav->muons) {
            if (muon.method != m_recname) continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;
            // if (has_stopping_in_cd_event[muon.ts]) continue;
            // if (has_stopping_in_wp_event[muon.ts]) continue;

            bool found_neutron = false;
            for (const vertex& neutron : m_nav->neutrons) {
                if (neutron.ts == prompt.ts || neutron.ts == delayed.ts) continue;
                if (neutron.ts < muon.ts + timestamp{0, 20000} || muon.ts + timestamp{0, 2000000} < neutron.ts) continue;
                found_neutron = true;
            }
            if (!found_neutron) continue;

            bool is_in_bkg = (
                muon.ts + m_ts_bkg_low <= prompt.ts && prompt.ts <= muon.ts + m_ts_bkg_high &&
                muon.ts + m_ts_bkg_low <= delayed.ts && delayed.ts <= muon.ts + m_ts_bkg_high
            );
            bool is_in_sig = (
                muon.ts + m_ts_sig_low <= prompt.ts && prompt.ts <= muon.ts + m_ts_sig_high &&
                muon.ts + m_ts_sig_low <= delayed.ts && delayed.ts <= muon.ts + m_ts_sig_high
            );
            if (!is_in_bkg && !is_in_sig) continue;

            vec3 dir = unit(muon.fpos - muon.ipos);
            double d_mu2p = mag(cross(dir, prompt.pos - muon.ipos));
            double d_mu2d = mag(cross(dir, delayed.pos - muon.ipos));
            if (m_radius < d_mu2p && m_radius < d_mu2d) continue;

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

protected:

    const energy_range_selection c_prompt_energy_cut{0.7, 12.0};
    const energy_range_selection c_delayed_energy_cut{2.0, 2.5};
    const fiducial_volume_selection c_fiducial_volume_cut{16500.0};
    const chimney_selection c_chimney_cut{15500.0, 2000.0};
    const flasher_selection c_flasher_cut{0.55, 0.45, 170.0, 80.0};

    const energy_range_selection c_multiplicity_energy_cut{2.0, 12.0};

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_