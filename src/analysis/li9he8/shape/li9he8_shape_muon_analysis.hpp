#ifndef JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONANALYSIS_HPP_

#include <set>

#include <TTree.h>

#include "analysis/analysis.hpp"
#include "event/cosmogenic.hpp"
#include "reader/navigator/ibd_like_event_correlated_chain_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "utils/muon_lookup.hpp"
#include "utils/scale_factor.hpp"

class li9he8_shape_muon_analysis : public analysis_base {

public:

    li9he8_shape_muon_analysis(
        const std::string& name, 
        const std::string& filepath, const std::string& suffix, 
        const std::string& reconstruction_filepath,
        const std::string& recname,
        const timestamp& sig_low, const timestamp& sig_high, 
        const timestamp& bkg_low, const timestamp& bkg_high, 
        double radius
    ) :
        analysis_base{name},
        m_recname{recname},
        m_ts_sig_low{sig_low},
        m_ts_sig_high{sig_high},
        m_ts_bkg_low{bkg_low},
        m_ts_bkg_high{bkg_high},
        m_radius{radius}
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

    virtual ~li9he8_shape_muon_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override = 0;

    bool process() override {
        if (m_is_sig) {
            m_cosmos_sig.insert({m_nav->run_id, m_nav->prompt, m_nav->delayed, m_dlat_mu2p_sig, m_dt_mu2p_sig, m_dlat_mu2d_sig, m_dt_mu2d_sig});
        }
        if (m_is_bkg) {
            m_cosmos_bkg.insert({m_nav->run_id, m_nav->prompt, m_nav->delayed, m_dlat_mu2p_bkg, m_dt_mu2p_bkg, m_dlat_mu2d_bkg, m_dt_mu2d_bkg});
        }
        return true;
    }

protected:

    std::shared_ptr<ibd_like_event_correlated_chain_navigator> m_nav;

    global_scale_factor_corrector m_gtc;

    std::string m_recname;
    timestamp m_ts_sig_low;
    timestamp m_ts_sig_high;
    timestamp m_ts_bkg_low;
    timestamp m_ts_bkg_high;
    double m_radius;

    double m_dlat_mu2p_sig;
    double m_dlat_mu2d_sig;
    double m_dt_mu2p_sig;
    double m_dt_mu2d_sig;
    bool m_is_sig;

    double m_dlat_mu2p_bkg;
    double m_dlat_mu2d_bkg;
    double m_dt_mu2p_bkg;
    double m_dt_mu2d_bkg;
    bool m_is_bkg;

    std::set<cosmogenic> m_cosmos_bkg, m_cosmos_sig;

    bool save_content() override {
        TTree* t_bkg = new TTree("background_events", "Cosmogenic depleted region");
        TTree* t_sig = new TTree("signal_events", "Cosmogenic enriched region");
        if (!t_bkg || !t_sig) {
            std::cerr << "Cannot create tree background or signal\n";
            return false;
        }
        int run_id;
        vec3 pos_p, pos_d;
        timestamp ts_p, ts_d;
        double e_p, e_d;
        double dlat_mu2p, dlat_mu2d; 
        double dt_mu2p, dt_mu2d;
        
        t_bkg->Branch("run_id", &run_id);
        t_bkg->Branch("posx_p", &pos_p.x);
        t_bkg->Branch("posy_p", &pos_p.y);
        t_bkg->Branch("posz_p", &pos_p.z);
        t_bkg->Branch("sec_p", &ts_p.sec);
        t_bkg->Branch("nsec_p", &ts_p.nsec);
        t_bkg->Branch("e_p", &e_p);
        t_bkg->Branch("dlat_mu2p", &dlat_mu2p);
        t_bkg->Branch("dt_mu2p", &dt_mu2p);
        t_bkg->Branch("posx_d", &pos_d.x);
        t_bkg->Branch("posy_d", &pos_d.y);
        t_bkg->Branch("posz_d", &pos_d.z);
        t_bkg->Branch("sec_d", &ts_d.sec);
        t_bkg->Branch("nsec_d", &ts_d.nsec);
        t_bkg->Branch("e_d", &e_d);
        t_bkg->Branch("dlat_mu2d", &dlat_mu2d);
        t_bkg->Branch("dt_mu2d", &dt_mu2d);

        for (std::set<cosmogenic>::const_iterator it = m_cosmos_bkg.begin(); it != m_cosmos_bkg.end(); ++it) {
            run_id = it->run_id;
            pos_p = it->prompt.pos;
            pos_d = it->delayed.pos;
            ts_p = it->prompt.ts;
            ts_d = it->delayed.ts;
            e_p = it->prompt.e / m_gtc.interpolate(it->prompt.ts);
            e_d = it->delayed.e / m_gtc.interpolate(it->delayed.ts);
            dlat_mu2p = it->dlat_mu2p;
            dlat_mu2d = it->dlat_mu2d;
            dt_mu2p = it->dt_mu2p;
            dt_mu2d = it->dt_mu2d;
            t_bkg->Fill();
        }

        t_sig->Branch("run_id", &run_id);
        t_sig->Branch("posx_p", &pos_p.x);
        t_sig->Branch("posy_p", &pos_p.y);
        t_sig->Branch("posz_p", &pos_p.z);
        t_sig->Branch("sec_p", &ts_p.sec);
        t_sig->Branch("nsec_p", &ts_p.nsec);
        t_sig->Branch("e_p", &e_p);
        t_sig->Branch("dlat_mu2p", &dlat_mu2p);
        t_sig->Branch("dt_mu2p", &dt_mu2p);
        t_sig->Branch("posx_d", &pos_d.x);
        t_sig->Branch("posy_d", &pos_d.y);
        t_sig->Branch("posz_d", &pos_d.z);
        t_sig->Branch("sec_d", &ts_d.sec);
        t_sig->Branch("nsec_d", &ts_d.nsec);
        t_sig->Branch("e_d", &e_d);
        t_sig->Branch("dlat_mu2d", &dlat_mu2d);
        t_sig->Branch("dt_mu2d", &dt_mu2d);

        for (std::set<cosmogenic>::const_iterator it = m_cosmos_sig.begin(); it != m_cosmos_sig.end(); ++it) {
            run_id = it->run_id;
            pos_p = it->prompt.pos;
            pos_d = it->delayed.pos;
            ts_p = it->prompt.ts;
            ts_d = it->delayed.ts;
            e_p = it->prompt.e / m_gtc.interpolate(it->prompt.ts);
            e_d = it->delayed.e / m_gtc.interpolate(it->delayed.ts);
            dlat_mu2p = it->dlat_mu2p;
            dlat_mu2d = it->dlat_mu2d;
            dt_mu2p = it->dt_mu2p;
            dt_mu2d = it->dt_mu2d;
            t_sig->Fill();
        }

        t_bkg->Write();
        t_sig->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_LI9HE8_LI9HE8SHAPEMUONANALYSIS_HPP_