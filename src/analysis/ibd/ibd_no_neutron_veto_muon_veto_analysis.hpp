#ifndef JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOMUONVETOANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOMUONVETOANALYSIS_HPP_

#include "analysis/ibd/ibd_analysis.hpp"
#include "selection/energy.hpp"
#include "selection/flasher.hpp"
#include "selection/spatial.hpp"
#include "selection/vertex.hpp"

class ibd_no_neutron_veto_muon_veto_analysis : public ibd_analysis {

public:

    ibd_no_neutron_veto_muon_veto_analysis(
        const std::string& name, 
        const std::string& filepath, const std::string& suffix,
        const std::string& reconstruction_filepath, 
        const std::string& recname,
        const timestamp& low, const timestamp& high, double radius
    ) :
        ibd_analysis{name, filepath, suffix, reconstruction_filepath},
        m_recname{recname},
        m_ts_low{low},
        m_ts_high{high},
        m_radius{radius}
    {}

    ~ibd_no_neutron_veto_muon_veto_analysis() override = default;

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{0, 5000}, timestamp{0, 1000000}};

        if (!c_prompt_energy_cut.is_in(prompt)) return false;
        if (!c_delayed_hydrogen_energy_cut.is_in(delayed) && !c_delayed_carbon_energy_cut.is_in(delayed)) return false;
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

        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        // stopping_muon_lookup has_stopping_in_cd_event;
        // stopping_muon_lookup has_stopping_in_wp_event;
        // has_stopping_in_cd_event.fill(m_nav->muons, "CdClassify");
        // has_stopping_in_wp_event.fill(m_nav->muons, "WpBasic");

        std::size_t nb_muon_veto = 0ul;
        for (const track& muon : m_nav->muons) {
            if (muon.method != m_recname) continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;
            // if (has_stopping_in_wp_event[muon.ts]) continue;

            bool found_neutron = false;
            for (const vertex& neutron : m_nav->neutrons) {
                vertex neu{neutron};
                neu.e /= m_gtc.interpolate(neu.ts);
                time_range_selection time_spallation_neutron_cut{muon.ts, timestamp{0, 20000}, timestamp{0, 2000000}};
                if (!time_spallation_neutron_cut.is_in(neu)) continue;
                found_neutron = true;
                break;
            }
            if (!found_neutron) continue;

            bool is_in_ts_veto = (
                muon.ts + m_ts_low < prompt.ts && prompt.ts < muon.ts + m_ts_high &&
                muon.ts + m_ts_low < delayed.ts && delayed.ts < muon.ts + m_ts_high
            );

            vec3 dir = unit(muon.fpos - muon.ipos);
            bool is_in_pos_veto = (
                mag(cross(dir, prompt.pos - muon.ipos)) < m_radius &&
                mag(cross(dir, delayed.pos - muon.ipos)) < m_radius
            );
            
            if (!is_in_ts_veto || !is_in_pos_veto) continue;
            ++nb_muon_veto;
        }
        if (nb_muon_veto) return false;

        calculate_dt_to_last_muon();
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

protected:

    const energy_range_selection c_prompt_energy_cut{0.7, 12.0};
    const energy_range_selection c_delayed_hydrogen_energy_cut{2.0, 2.5};
    const energy_range_selection c_delayed_carbon_energy_cut{4.5, 5.5};
    const fiducial_volume_selection c_fiducial_volume_cut{17200.0};
    const chimney_selection c_chimney_cut{15500.0, 2000.0};
    const flasher_selection c_flasher_cut{0.55, 0.45, 170.0, 80.0};

    const energy_range_selection c_multiplicity_energy_cut{2.0, 12.0};

    std::string m_recname;
    timestamp m_ts_low;
    timestamp m_ts_high;
    double m_radius;

};

#endif // JRAFNECK_ANALYSIS_IBD_IBDNONEUTRONVETOMUONVETOANALYSIS_HPP_