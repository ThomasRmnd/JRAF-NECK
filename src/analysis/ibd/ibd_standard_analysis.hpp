#ifndef JRAFNECK_ANALYSIS_IBD_IBDSTANDARDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_IBD_IBDSTANDARDANALYSIS_HPP_

#include "analysis/ibd/ibd_analysis.hpp"
#include "selection/constants.hpp"

class ibd_standard_analysis : public ibd_analysis {

public:

    using ibd_analysis::ibd_analysis;

    ~ibd_standard_analysis() override = default;

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

        std::size_t nb_neutron_veto = 0ul;
        for (const vertex& neutron : m_nav->neutrons) {
            if (neutron.ts == prompt.ts || neutron.ts == delayed.ts) continue;
            if (!g_acrylic_sphere_cut.is_in(neutron)) continue;
            vertex neu{neutron};
            neu.e /= m_gtc.interpolate(neu.ts);
            if (!g_neutron_energy_cut.is_in(neu)) continue;
            if (neutron.stdt > 275.0) continue; // flasher cut
            vertex_correlation_selection vertex_correlation_neutron_cut{neu, 4000.0, timestamp{0, 20000}, timestamp{0, 1200000000}};
            if (!vertex_correlation_neutron_cut.is_in(prompt) && !vertex_correlation_neutron_cut.is_in(delayed)) continue;
            ++nb_neutron_veto;
        }
        if (nb_neutron_veto) return false;

        calculate_dt_to_last_muon();
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_IBD_IBDSTANDARDANALYSIS_HPP_