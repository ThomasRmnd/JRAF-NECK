#ifndef JRAFNECK_ANLYSIS_ACCIDENTAL_ACCIDENTALSTANDARDMUONVETOANALYSIS_HPP_
#define JRAFNECK_ANLYSIS_ACCIDENTAL_ACCIDENTALSTANDARDMUONVETOANALYSIS_HPP_

#include "analysis/accidental/accidental_analysis.hpp"
#include "selection/constants.hpp"

class accidental_standard_muon_veto_analysis : public accidental_analysis {

public:

    accidental_standard_muon_veto_analysis(
        const std::string& name, 
        const std::string& filepath, const std::string& suffix,
        const std::string& reconstruction_filepath, 
        const std::string& recname,
        const timestamp& low, const timestamp& high, double radius
    ) :
        accidental_analysis{name, filepath, suffix, reconstruction_filepath},
        m_recname{recname},
        m_ts_low{low},
        m_ts_high{high},
        m_radius{radius}
    {}

    bool selection() override {
        vertex prompt{m_nav->prompt};
        vertex delayed{m_nav->delayed};
        prompt.e /= m_gtc.interpolate(prompt.ts);
        delayed.e /= m_gtc.interpolate(delayed.ts);

        vertex_correlation_selection vertex_correlation_cut{prompt, 1500.0, timestamp{2, 0}, timestamp{4, 0}};

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
            if (
                (mult.ts < prompt.ts - timestamp{0, 1000000} || prompt.ts                          < mult.ts) &&
                (mult.ts < delayed.ts                        || delayed.ts + timestamp{0, 1000000} < mult.ts)
            ) continue;
            ++nb_multu_veto;
        }
        if (nb_multu_veto) return false;

        std::size_t nb_neutron_veto = 0ul;
        for (const vertex& neutron : m_nav->neutrons) {
            if (neutron.ts == prompt.ts || neutron.ts == delayed.ts) continue;
            vertex neu{neutron};
            neu.e /= m_gtc.interpolate(neu.ts);
            if (!g_neutron_energy_cut.is_in(neu)) continue;
            if (!g_acrylic_sphere_cut.is_in(neu)) continue;
            if (neutron.stdt > 275.0) continue; // flasher cut
            vertex_correlation_selection vertex_correlation_neutron_cut{neu, 4000.0, timestamp{0, 20000}, timestamp{0, 1200000000}};
            if (!vertex_correlation_neutron_cut.is_in(prompt)) continue;
            ++nb_neutron_veto;
        }
        if (nb_neutron_veto) return false;

        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        std::size_t nb_muon_veto = 0ul;
        for (const track& muon : m_nav->muons) {
            if (muon.method != m_recname) continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;

            bool is_in_ts_veto = (
                muon.ts + m_ts_low < prompt.ts && prompt.ts < muon.ts + m_ts_high
            );

            vec3 dir = unit(muon.fpos - muon.ipos);
            bool is_in_pos_veto = (
                mag(cross(dir, prompt.pos - muon.ipos)) < m_radius
            );
            
            if (!is_in_ts_veto || !is_in_pos_veto) continue;
            ++nb_muon_veto;
        }
        if (nb_muon_veto) return false;

        dt_to_last_muon_result res = calculate_dt_to_last_muon_with_neutron(prompt, m_nav->muons, m_nav->neutrons);
        m_dt_last_mu_with_neu = res.dt_last_mu;
        if (!res.is_set) {
            m_dt_last_mu_with_neu = timestamp{-1, 0};
        }
        calculate_dlat_dt_muon_to_prompt();

        return true;
    }

protected:

    std::string m_recname;
    timestamp m_ts_low;
    timestamp m_ts_high;
    double m_radius;

};  

#endif // JRAFNECK_ANLYSIS_ACCIDENTAL_ACCIDENTALSTANDARDMUONVETOANALYSIS_HPP_