#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONANALYSIS_HPP_

#include <array>
#include <set>

#include <TTree.h>

#include "analysis/analysis.hpp"
#include "reader/navigator/ibd_like_event_correlated_chain_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "selection/constants.hpp"
#include "utils/muon.hpp"
#include "utils/scale_factor.hpp"

const std::size_t nb_radius = 10ul;

struct cosmogenic_with_rate {

    int run_id;
    vertex prompt;
    double dlat_mu2p;
    timestamp dt_mu2p;
    vertex delayed;
    double dlat_mu2d;
    timestamp dt_mu2d;
    timestamp dt_last_mu_with_neu;
    timestamp dt_last_mu;
    std::array<timestamp, nb_radius> dt_last_mu_with_neu_radius;
    std::array<timestamp, nb_radius> dt_last_mu_radius;

    cosmogenic_with_rate() = default;
    cosmogenic_with_rate(const cosmogenic_with_rate&) = default;
    cosmogenic_with_rate(cosmogenic_with_rate&&) = default;
    cosmogenic_with_rate& operator=(const cosmogenic_with_rate&) = default;
    cosmogenic_with_rate& operator=(cosmogenic_with_rate&&) = default;

    cosmogenic_with_rate(int run_id_, const vertex& prompt_, const vertex& delayed_, double dlat_mu2p_, const timestamp& dt_mu2p_, double dlat_mu2d_, const timestamp& dt_mu2d_, const timestamp& dt_last_mu_with_neu_, const timestamp& dt_last_mu_) :
        run_id{run_id_},
        prompt{prompt_},
        dlat_mu2p{dlat_mu2p_},
        dt_mu2p{dt_mu2p_},
        delayed{delayed_},
        dlat_mu2d{dlat_mu2d_},
        dt_mu2d{dt_mu2d_},
        dt_last_mu_with_neu{dt_last_mu_with_neu_},
        dt_last_mu{dt_last_mu_}
    {}

};

inline bool operator<(const cosmogenic_with_rate& lhs, const cosmogenic_with_rate& rhs) {
    return lhs.prompt.ts < rhs.prompt.ts;
}

class li9he8_rate_muon_analysis : public analysis_base {

public:

    li9he8_rate_muon_analysis
(
        const std::string& name, 
        const std::string& filepath, const std::string& suffix, 
        const std::string& reconstruction_filepath
    ) :
        analysis_base{name}
    {
        std::string treename = "IBDAnalysis" + suffix;
        m_nav = navigator_manager::retrieve<ibd_like_event_correlated_chain_navigator>(
            filepath, treename, 
            filepath, "NeutronAnalysis" + suffix, 
            filepath, "MultiplicityAnalysis" + suffix, 
            reconstruction_filepath, "muons"
        );
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        if (!m_gtc.load()) {
            std::cerr << "Cannot load global time corrector\n";
            return;
        }
    }

    virtual ~li9he8_rate_muon_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

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

    bool process() override {
        cosmogenic_with_rate c{
            m_nav->run_id, m_nav->prompt, m_nav->delayed,
            m_dlat_mu2p, m_dt_mu2p, m_dlat_mu2d, m_dt_mu2d,
            m_dt_last_mu_with_neu, m_dt_last_mu
        };
        c.dt_last_mu_with_neu_radius = m_dt_last_mu_with_neu_radius;
        c.dt_last_mu_radius = m_dt_last_mu_radius;
        m_cosmos.insert(c);
        return true;
    }

protected:

    std::shared_ptr<ibd_like_event_correlated_chain_navigator> m_nav;

    global_scale_factor_corrector m_gtc;

    std::set<cosmogenic_with_rate> m_cosmos;
    double m_dlat_mu2p;
    timestamp m_dt_mu2p;
    double m_dlat_mu2d;
    timestamp m_dt_mu2d;
    timestamp m_dt_last_mu_with_neu;
    timestamp m_dt_last_mu;
    std::array<timestamp, nb_radius> m_dt_last_mu_with_neu_radius;
    std::array<timestamp, nb_radius> m_dt_last_mu_radius;

    bool save_content() override {
        TTree* t = new TTree("events", "Events");
        if (!t) {
            std::cerr << "Cannot create tree events\n";
            return false;
        }
        int run_id;
        vec3 pos_p, pos_d;
        timestamp ts_p, ts_d;
        double e_p, e_d;
        double dlat_mu2p;
        double dt_mu2p;
        double dlat_mu2d;
        double dt_mu2d;
        double dt_last_mu_with_neu;
        double dt_last_mu;
        std::array<double, nb_radius> dt_last_mu_with_neu_radius;
        std::array<double, nb_radius> dt_last_mu_radius;
        t->Branch("run_id", &run_id);
        t->Branch("posx_p", &pos_p.x);
        t->Branch("posy_p", &pos_p.y);
        t->Branch("posz_p", &pos_p.z);
        t->Branch("sec_p", &ts_p.sec);
        t->Branch("nsec_p", &ts_p.nsec);
        t->Branch("e_p", &e_p);
        t->Branch("posx_d", &pos_d.x);
        t->Branch("posy_d", &pos_d.y);
        t->Branch("posz_d", &pos_d.z);
        t->Branch("sec_d", &ts_d.sec);
        t->Branch("nsec_d", &ts_d.nsec);
        t->Branch("e_d", &e_d);
        t->Branch("dlat_mu2p", &dlat_mu2p);
        t->Branch("dt_mu2p", &dt_mu2p);
        t->Branch("dlat_mu2d", &dlat_mu2d);
        t->Branch("dt_mu2d", &dt_mu2d);
        t->Branch("dt_last_mu_with_neu", &dt_last_mu_with_neu);
        t->Branch("dt_last_mu", &dt_last_mu);
        for (std::size_t k = 0ul; k < nb_radius; ++k) {
            t->Branch(("dt_last_mu_with_neu_" + std::to_string(k + 1ul) + "m").c_str(), &dt_last_mu_with_neu_radius[k]);
            t->Branch(("dt_last_mu_" + std::to_string(k + 1ul) + "m").c_str(), &dt_last_mu_radius[k]);
        }

        for (std::set<cosmogenic_with_rate>::const_iterator it = m_cosmos.begin(); it != m_cosmos.end(); ++it) {
            run_id = it->run_id;
            pos_p = it->prompt.pos;
            pos_d = it->delayed.pos;
            ts_p = it->prompt.ts;
            ts_d = it->delayed.ts;
            e_p = it->prompt.e / m_gtc.interpolate(it->prompt.ts);
            e_d = it->delayed.e / m_gtc.interpolate(it->delayed.ts);
            dlat_mu2p = it->dlat_mu2p;
            dt_mu2p = timestamp_to_double(it->dt_mu2p);
            dlat_mu2d = it->dlat_mu2d;
            dt_mu2d = timestamp_to_double(it->dt_mu2d);
            dt_last_mu_with_neu = timestamp_to_double(it->dt_last_mu_with_neu);
            dt_last_mu = timestamp_to_double(it->dt_last_mu);
            for (std::size_t k = 0ul; k < nb_radius; ++k) {
                dt_last_mu_with_neu_radius[k] = timestamp_to_double(it->dt_last_mu_with_neu_radius[k]);
                dt_last_mu_radius[k] = timestamp_to_double(it->dt_last_mu_radius[k]);
            }
            t->Fill();
        }

        return true;
    }

    void calculate_dlat_dt_muon_to_prompt() {
        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        m_dlat_mu2p = std::numeric_limits<double>::infinity();
        m_dt_mu2p = timestamp{0, 0};
        m_dlat_mu2d = std::numeric_limits<double>::infinity();
        m_dt_mu2d = timestamp{0, 0};
        bool is_set_dlat_mu2p = false;
        for (const track& muon : m_nav->muons) {
            if (muon.method != "CdWpTtChi2") continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;

            bool is_in_sig = (
                muon.ts + timestamp{0, 5000000} <= m_nav->prompt.ts && m_nav->prompt.ts <= muon.ts + timestamp{0, 1200000000} &&
                muon.ts + timestamp{0, 5000000} <= m_nav->delayed.ts && m_nav->delayed.ts <= muon.ts + timestamp{0, 1200000000}
            );
            if (!is_in_sig) continue;

            vec3 dir = unit(muon.fpos - muon.ipos);
            double d_mu2p = mag(cross(dir, m_nav->prompt.pos - muon.ipos));
            double d_mu2d = mag(cross(dir, m_nav->delayed.pos - muon.ipos));

            if (m_dlat_mu2p < d_mu2p) continue;
            m_dlat_mu2p = d_mu2p;
            m_dt_mu2p = m_nav->prompt.ts - muon.ts;
            m_dlat_mu2d = d_mu2d;
            m_dt_mu2d = m_nav->delayed.ts - muon.ts;
            is_set_dlat_mu2p = true;
        }

        if (!is_set_dlat_mu2p) {
            m_dlat_mu2p = -1.0;
            m_dt_mu2p = timestamp{-1, 0};
            m_dlat_mu2d = -1.0;
            m_dt_mu2d = timestamp{-1, 0};
        }
    }

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8RATEMUONANALYSIS_HPP_