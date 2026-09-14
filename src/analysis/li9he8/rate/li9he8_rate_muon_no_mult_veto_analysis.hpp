#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONNOMULTVETOANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONNOMULTVETOANALYSIS_HPP_

#include "analysis/li9he8/rate/li9he8_rate_muon_analysis.hpp"
#include "selection/constants.hpp"
#include "utils/muon.hpp"

class li9he8_rate_muon_no_mult_veto_analysis : public li9he8_rate_muon_analysis {

public:

    using li9he8_rate_muon_analysis::li9he8_rate_muon_analysis;

    virtual ~li9he8_rate_muon_no_mult_veto_analysis() override = default;

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        if (!g_prompt_energy_cut.is_in(prompt)) return false;
        if (!g_delayed_hydrogen_energy_cut.is_in(delayed) && !g_delayed_carbon_energy_cut.is_in(delayed)) return false;
        if (!g_fiducial_volume_cut.is_in(prompt)) return false;
        if (!g_acrylic_sphere_cut.is_in(delayed)) return false;
        if (g_chimney_cut.is_in(prompt)) return false;
        if (!g_flasher_cut.is_in(prompt)) return false;

        const double r = mag(prompt.pos);
        if (r <= 16500.0) {
            vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{0, 5000}, timestamp{0, 1000000}}; // 1.5 m & [5 us, 1 ms]
            if (!vertex_correlation_cut.is_in(delayed)) return false;
        }
        else {
            vertex_correlation_selection vertex_correlation_cut{prompt, 750.0, timestamp{0, 5000}, timestamp{0, 600000}}; // 0.75 m & [5 us, 0.6 ms]
            if (!vertex_correlation_cut.is_in(delayed)) return false;
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

        for (std::size_t k = 0ll; k < nb_radius; ++k) {
            double radius = static_cast<double>(k + 1ll) * 1000.0;
            res = calculate_dt_to_last_muon_with_neutron_within_cylinder(prompt, m_nav->muons, m_nav->neutrons, radius);
            m_dt_last_mu_with_neu_radius[k] = res.dt_last_mu;
            if (!res.is_set) {
                m_dt_last_mu_with_neu_radius[k] = timestamp{-1, 0};
            }
            res = calculate_dt_to_last_muon_within_cylinder(prompt, m_nav->muons, radius);
            m_dt_last_mu_radius[k] = res.dt_last_mu;
            if (!res.is_set) {
                m_dt_last_mu_radius[k] = timestamp{-1, 0};
            }
        }
        
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONNOMULTVETOANALYSIS_HPP_