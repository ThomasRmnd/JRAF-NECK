#ifndef ANALYSIS_IBD_STANDARD_ANALYSIS_HPP_
#define ANALYSIS_IBD_STANDARD_ANALYSIS_HPP_

#include <set>

#include "analysis/ibd/ibd_analysis.hpp"
#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/spatial.hpp"
#include "selection/vertex.hpp"

class ibd_standard_analysis : public ibd_analysis {

public:

    using ibd_analysis::ibd_analysis;

    virtual ~ibd_standard_analysis() override = default;

    virtual bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(m_nav->prompt.ts);
        delayed.e /= m_gtc.interpolate(m_nav->delayed.ts);

        energy_range_selection prompt_energy_cut{0.7, 12.0};
        energy_range_selection delayed_energy_cut{2.0, 2.5};
        fiducial_volume_selection fiducial_volume_cut{16500.0};
        chimney_selection chimney_cut{15500.0, 2000.0};
        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{0, 5000}, timestamp{0, 1000000}};
        flasher_selection flasher_cut{0.55, 0.45, 170.0, 80.0};

        energy_range_selection multiplicity_energy_cut{2.0, 12.0};

        energy_range_selection neutron_energy_cut{1.5, 20.0};
        vertex_correlation_selection vertex_correlation_neutron_prompt_cut{prompt, 4000.0, timestamp{0, -1200000000}, timestamp{0, -20000}};
        vertex_correlation_selection vertex_correlation_neutron_delayed_cut{delayed, 4000.0, timestamp{0, -1200000000}, timestamp{0, -20000}};

        if (!prompt_energy_cut.is_in(prompt)) return false;
        if (!delayed_energy_cut.is_in(delayed)) return false;
        if (!fiducial_volume_cut.is_in(prompt)) return false;
        if (!chimney_cut.is_in(prompt)) return false;
        if (!vertex_correlation_cut.is_in(delayed)) return false;
        if (!flasher_cut.is_in(prompt)) return false;

        std::size_t nb_multu_veto = 0ul;
        for (const vertex& multiplicity : m_nav->multiplicities) {
            vertex mult{multiplicity};
            mult.e /= m_gtc.interpolate(mult.ts);
            if (!multiplicity_energy_cut.is_in(mult)) continue;
            if (mult.ts < prompt.ts - timestamp{0, 1000000} || delayed.ts + timestamp{0, 1000000} < mult.ts) continue;
            ++nb_multu_veto;
        }
        if (nb_multu_veto) return false;

        std::size_t nb_neutron_veto = 0ul;
        for (const vertex& neutron : m_nav->neutrons) {
            vertex neu{neutron};
            neu.e /= m_gtc.interpolate(neu.ts);
            if (!neutron_energy_cut.is_in(neu)) continue;
            if (neutron.stdt > 275.0) continue; // flasher cut
            if (!vertex_correlation_neutron_prompt_cut.is_in(neu) && !vertex_correlation_neutron_delayed_cut.is_in(neu)) continue; // reverse correlation
            ++nb_neutron_veto;
        }
        if (nb_neutron_veto) return false;

        calculate_dt_to_last_muon();
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

};

#endif // ANALYSIS_IBD_STANDARD_ANALYSIS_HPP_