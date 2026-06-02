#ifndef JRAFNECK_ANALYSIS_IBD_IBDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_IBD_IBDANALYSIS_HPP_

#include <set>

#include <TTree.h>

#include "analysis/analysis.hpp"
#include "event/ibd.hpp"
#include "reader/navigator/ibd_like_event_navigator.hpp"
#include "reader/navigator/ibd_like_event_correlated_chain_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "utils/muon_lookup.hpp"
#include "utils/scale_factor.hpp"

struct ibd_wmu {

    ibd i;
    timestamp dt_last_mu;
    double dlat_mu2p; // to closest muon
    timestamp dt_mu2p; // to closest muon

};

inline bool operator<(const ibd_wmu& lhs, const ibd_wmu& rhs) {
    return lhs.i < rhs.i;
}

class ibd_analysis : public analysis_base {

public:

    ibd_analysis(
        const std::string& name, 
        const std::string& filepath, const std::string& suffix, 
        const std::string& reconstruction_filepath
    ) :
        analysis_base{name}
    {
        std::string treename = "IBDAnalysis" + suffix;
        // m_nav = navigator_manager::retrieve<ibd_like_event_correlated_chain_navigator>(
        //     filepath, treename, 
        //     filepath, "NeutronAnalysis" + suffix, 
        //     filepath, "MultiplicityAnalysis" + suffix, 
        //     reconstruction_filepath, "muons"
        // );
        m_nav = navigator_manager::retrieve<ibd_like_event_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        if (!m_gtc.load()) {
            std::cerr << "Cannot load global time corrector\n";
            return;
        }
    }

    virtual ~ibd_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    virtual bool selection() override = 0;

    bool process() override {
        m_ibds.insert({
            {m_nav->run_id, m_nav->prompt, m_nav->delayed}, 
            m_dt_last_mu, m_dlat_mu2p, m_dt_mu2p
        });
        return true;
    }

protected:

    // std::shared_ptr<ibd_like_event_correlated_chain_navigator> m_nav;
    std::shared_ptr<ibd_like_event_navigator> m_nav;

    global_scale_factor_corrector m_gtc;

    std::set<ibd_wmu> m_ibds;
    timestamp m_dt_last_mu;
    double m_dlat_mu2p;
    timestamp m_dt_mu2p;

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
        double dt_last_mu;
        double dlat_mu2p;
        double dt_mu2p;
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
        t->Branch("dt_last_mu", &dt_last_mu);
        t->Branch("dlat_mu2p", &dlat_mu2p);
        t->Branch("dt_mu2p", &dt_mu2p);

        for (std::set<ibd_wmu>::const_iterator it = m_ibds.begin(); it != m_ibds.end(); ++it) {
            run_id = it->i.run_id;
            pos_p = it->i.prompt.pos;
            pos_d = it->i.delayed.pos;
            ts_p = it->i.prompt.ts;
            ts_d = it->i.delayed.ts;
            e_p = it->i.prompt.e / m_gtc.interpolate(it->i.prompt.ts);
            e_d = it->i.delayed.e / m_gtc.interpolate(it->i.delayed.ts);
            dt_last_mu = timestamp_to_double(it->dt_last_mu);
            dlat_mu2p = it->dlat_mu2p;
            dt_mu2p = timestamp_to_double(it->dt_mu2p);
            t->Fill();
        }

        return true;
    }

    void calculate_dt_to_last_muon() {
        m_dt_last_mu = timestamp{0, 0};
        bool is_set_dt_last_mu = false;
        for (const track& muon : m_nav->muons) {
            if (m_nav->prompt.ts < muon.ts) continue;
            bool found_neutron = false;
            for (const vertex& neutron : m_nav->neutrons) {
                if (neutron.ts < muon.ts + timestamp{0, 20000} || muon.ts + timestamp{0, 2000000} < neutron.ts) continue;
                found_neutron = true;
                break;
            }
            if (!found_neutron) continue;
            if (is_set_dt_last_mu && m_nav->prompt.ts - muon.ts > m_dt_last_mu) continue;
            m_dt_last_mu = m_nav->prompt.ts - muon.ts;
            is_set_dt_last_mu = true;
        }
        if (!is_set_dt_last_mu) {
            m_dt_last_mu = timestamp{-1, 0};
        }
    }

    void calculate_dlat_dt_muon_to_prompt() {
        multiplicity_muon_lookup nb_muons_in_cd_event;
        multiplicity_muon_lookup nb_muons_in_wp_event;
        nb_muons_in_cd_event.fill(m_nav->muons, "CdClassify");
        nb_muons_in_wp_event.fill(m_nav->muons, "WpBasic");

        // stopping_muon_lookup has_stopping_in_cd_event;
        // stopping_muon_lookup has_stopping_in_wp_event;
        // has_stopping_in_cd_event.fill(m_nav, "CdClassify");
        // has_stopping_in_wp_event.fill(m_nav, "WpBasic");

        m_dlat_mu2p = std::numeric_limits<double>::infinity();
        m_dt_mu2p = timestamp{0, 0};
        bool is_set_dlat_mu2p = false;
        for (const track& muon : m_nav->muons) {
            if (muon.method != "CdWpTtChi2") continue;
            if (nb_muons_in_cd_event[muon.ts] > 1ul || nb_muons_in_wp_event[muon.ts] > 1ul) continue;
            // if (has_stopping_in_cd_event[muon.ts]) continue;
            // if (has_stopping_in_wp_event[muon.ts]) continue;

            bool is_in_sig = (
                muon.ts + timestamp{0, 5000000} <= m_nav->prompt.ts && m_nav->prompt.ts <= muon.ts + timestamp{0, 1200000000} &&
                muon.ts + timestamp{0, 5000000} <= m_nav->delayed.ts && m_nav->delayed.ts <= muon.ts + timestamp{0, 1200000000}
            );
            if (!is_in_sig) continue;

            vec3 dir = unit(muon.fpos - muon.ipos);
            double d_mu2p = mag(cross(dir, m_nav->prompt.pos - muon.ipos));

            if (m_dlat_mu2p < d_mu2p) continue;
            m_dlat_mu2p = d_mu2p;
            m_dt_mu2p = m_nav->prompt.ts - muon.ts;
            is_set_dlat_mu2p = true;
        }

        if (!is_set_dlat_mu2p) {
            m_dlat_mu2p = -1.0;
            m_dt_mu2p = timestamp{-1, 0};
        }
    }

};

#endif // JRAFNECK_ANALYSIS_IBD_IBDANALYSIS_HPP_