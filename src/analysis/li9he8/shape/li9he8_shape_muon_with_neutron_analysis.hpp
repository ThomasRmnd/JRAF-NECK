#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_

#include "analysis/li9he8/shape/li9he8_shape_muon_analysis.hpp"
#include "selection/constants.hpp"

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
            if (!g_acrylic_sphere_cut.is_in(multiplicity)) continue;
            vertex mult{multiplicity};
            mult.e /= m_gtc.interpolate(mult.ts);
            if (!g_multiplicity_energy_cut.is_in(mult)) continue;
            if (mult.ts < prompt.ts - timestamp{0, 1000000} || delayed.ts + timestamp{0, 1000000} < mult.ts) continue;
            ++nb_multu_veto;
        }
        if (nb_multu_veto) return false;

        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        m_dlat_mu2p_sig = std::numeric_limits<double>::infinity();
        m_dlat_mu2d_sig = std::numeric_limits<double>::infinity();
        m_dt_mu2p_sig = timestamp{-1, 0};
        m_dt_mu2d_sig = timestamp{-1, 0};
        m_is_sig = false;

        m_dlat_mu2p_bkg = std::numeric_limits<double>::infinity();
        m_dlat_mu2d_bkg = std::numeric_limits<double>::infinity();
        m_dt_mu2p_bkg = timestamp{-1, 0};
        m_dt_mu2d_bkg = timestamp{-1, 0};
        m_is_bkg = false;

        for (const track& muon : m_nav->muons) {
            if (muon.method != m_recname) continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;

            bool found_neutron = false;
            for (const vertex& neutron : m_nav->neutrons) {
                if (neutron.ts == prompt.ts || neutron.ts == delayed.ts) continue;
                if (!g_acrylic_sphere_cut.is_in(neutron)) continue;
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

            vec3 dir = unit(muon.fpos - muon.ipos);
            double d_mu2p = mag(cross(dir, prompt.pos - muon.ipos));
            double d_mu2d = mag(cross(dir, delayed.pos - muon.ipos));
            const timestamp dt_mu2p = prompt.ts - muon.ts;
            const timestamp dt_mu2d = delayed.ts - muon.ts;

            if (is_in_sig && d_mu2p < m_radius && d_mu2p < m_dlat_mu2p_sig) {
                m_dlat_mu2p_sig = d_mu2p;
                m_dlat_mu2d_sig = d_mu2d;
                m_dt_mu2p_sig = dt_mu2p;
                m_dt_mu2d_sig = dt_mu2d;
                m_is_sig = true;
            }

            if (is_in_bkg && d_mu2p < m_radius && d_mu2p < m_dlat_mu2p_bkg) {
                m_dlat_mu2p_bkg = d_mu2p;
                m_dlat_mu2d_bkg = d_mu2d;
                m_dt_mu2p_bkg = dt_mu2p;
                m_dt_mu2d_bkg = dt_mu2d;
                m_is_bkg = true;
            }
        }

        dt_to_last_muon_result res = calculate_dt_to_last_muon_with_neutron(prompt, m_nav->muons, m_nav->neutrons);
        m_dt_last_mu_with_neu = res.dt_last_mu;
        if (!res.is_set) {
            m_dt_last_mu_with_neu = timestamp{-1, 0};
        }
        res = calculate_dt_to_last_muon(prompt, m_nav->muons);
        m_dt_last_mu = res.dt_last_mu;
        if (!res.is_set) {
            m_dt_last_mu = timestamp{-1, 0};
        }

        return m_is_sig || m_is_bkg;
    }

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONWITHNEUTRONANALYSIS_HPP_