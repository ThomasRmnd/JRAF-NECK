#ifndef JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOANALYSIS_HPP_

#include "analysis/ibd/ibd_analysis.hpp"
#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/spatial.hpp"
#include "selection/vertex.hpp"

class ibd_no_neutron_veto_analysis : public ibd_analysis {

public:

    ibd_no_neutron_veto_analysis(const std::string& name, const std::string& filepath, const std::string& suffix) :
        ibd_analysis{name, filepath, suffix}
    {}

    ~ibd_no_neutron_veto_analysis() override = default;

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
        std::cout << "[Debug] Event: " << nb_multu_veto << " multiplicity vetoes (over " << m_nav->multiplicities.size() << ")\n";
        if (nb_multu_veto) return false;

        calculate_dt_to_last_muon();
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

protected:

    const energy_range_selection c_prompt_energy_cut{0.7, 12.0};
    const energy_range_selection c_delayed_energy_cut{2.0, 2.5};
    const fiducial_volume_selection c_fiducial_volume_cut{16500.0};
    const chimney_selection c_chimney_cut{15500.0, 2000.0};
    const flasher_selection c_flasher_cut{0.55, 0.45, 170.0, 80.0};

    const energy_range_selection c_multiplicity_energy_cut{2.0, 12.0};

};

#endif // JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOANALYSIS_HPP_