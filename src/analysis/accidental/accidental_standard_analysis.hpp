#ifndef JRAFNECK_ANALYSIS_ACCIDENTAL_ACCIDENTALSTANDARDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_ACCIDENTAL_ACCIDENTALSTANDARDANALYSIS_HPP_

#include "analysis/accidental/accidental_analysis.hpp"
#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/spatial.hpp"
#include "selection/vertex.hpp"

class accidental_standard_analysis : public accidental_analysis {

public:

    using accidental_analysis::accidental_analysis;

    ~accidental_standard_analysis() override = default;

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{2, 0}, timestamp{4, 0}};

        if (!c_prompt_energy_cut.is_in(prompt)) return false;
        if (!c_delayed_hydrogen_energy_cut.is_in(delayed) && !c_delayed_carbon_energy_cut.is_in(delayed)) return false;
        if (!c_fiducial_volume_cut.is_in(prompt)) return false;
        if (!c_acrylic_sphere_cut.is_in(delayed)) return false;
        if (c_chimney_cut.is_in(prompt)) return false;
        if (!vertex_correlation_cut.is_in(delayed)) return false;
        if (!c_flasher_cut.is_in(prompt)) return false;

        // std::size_t nb_multu_veto = 0ul;
        // for (const vertex& multiplicity : m_nav->multiplicities) {
        //     if (multiplicity.ts == prompt.ts || multiplicity.ts == delayed.ts) continue;
        //     vertex mult{multiplicity};
        //     mult.e /= m_gtc.interpolate(mult.ts);
        //     if (!c_multiplicity_energy_cut.is_in(mult)) continue;
        //     if (mult.ts < prompt.ts - timestamp{0, 1000000} || delayed.ts + timestamp{0, 1000000} < mult.ts) continue;
        //     ++nb_multu_veto;
        // }
        // if (nb_multu_veto) return false;

        std::size_t nb_neutron_veto = 0ul;
        for (const vertex& neutron : m_nav->neutrons) {
            if (neutron.ts == prompt.ts || neutron.ts == delayed.ts) continue;
            vertex neu{neutron};
            neu.e /= m_gtc.interpolate(neu.ts);
            if (!c_neutron_energy_cut.is_in(neu)) continue;
            if (neutron.stdt > 275.0) continue; // flasher cut
            vertex_correlation_selection vertex_correlation_neutron_cut{neu, 4000.0, timestamp{0, 20000}, timestamp{0, 1200000000}};
            if (!vertex_correlation_neutron_cut.is_in(prompt)) continue;
            ++nb_neutron_veto;
        }
        if (nb_neutron_veto) return false;

        calculate_dt_to_last_muon();
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

protected:

    const energy_range_selection c_prompt_energy_cut{0.7, 12.0};
    const energy_range_selection c_delayed_hydrogen_energy_cut{2.0, 2.5};
    const energy_range_selection c_delayed_carbon_energy_cut{4.5, 5.5};
    const fiducial_volume_selection c_fiducial_volume_cut{17200.0};
    const fiducial_volume_selection c_acrylic_sphere_cut{17700.0};
    const chimney_selection c_chimney_cut{15500.0, 2000.0};
    const flasher_selection c_flasher_cut{0.55, 0.45, 170.0, 80.0};

    const energy_range_selection c_multiplicity_energy_cut{2.0, 12.0};

    const energy_range_selection c_neutron_energy_cut{1.5, 20.0};

};

#endif // JRAFNECK_ANALYSIS_ACCIDENTAL_ACCIDENTALSTANDARDANALYSIS_HPP_