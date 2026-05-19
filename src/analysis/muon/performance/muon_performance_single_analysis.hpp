#ifndef JRAFNECK_ANALYSIS_MUON_PERFORMANCE_MUONPERFORMANCESINGLEANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_PERFORMANCE_MUONPERFORMANCESINGLEANALYSIS_HPP_

#include "analysis/analysis.hpp"
// #include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/muon_event_correlated_chain_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"

class muon_performance_single_analysis : public analysis_base {

public:

    muon_performance_single_analysis(const std::string& name, const std::string& filepath, const std::string& treename, const std::string& targetname, const std::string& refname) :
        analysis_base{name},
        m_targetname{targetname},
        m_refname{refname}
    {
        // m_nav = navigator_manager::retrieve<muon_event_correlated_chain_navigator>(filepath, treename);
        m_nav = navigator_manager::retrieve<muon_event_correlated_chain_navigator>(
            filepath, treename, 
            "/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/Amber_v5.5/muonReco_Amber_v5.5_run9789.root", "MuonReco",
            "/sps/juno/jdeandre/rtraw_ThomasRaymond/reconstruction/reprod/EDWIN/RUN9789-user.root", "Edwin_Muon",
            "root://xrootd-archive.cr.cnaf.infn.it:1095//production/storm/dirac/juno/user/j/jpandre_1/tt_data_auto/2025/0830/RUN.9789.*.EDM.user.root", "TT"
        );
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }

        m_tree = new TTree("performance", "Muon reconstruction performance");
        if (!m_tree) {
            std::cerr << "Cannot create tree performance\n";
            return;
        }
        m_tree->Branch("sec", &m_sec);
        m_tree->Branch("nsec", &m_nsec);
        m_tree->Branch("totq_cd", &m_totq_cd);
        m_tree->Branch("totq_wp", &m_totq_wp);
        m_tree->Branch("angle", &m_angle);
        m_tree->Branch("distance", &m_distance);
        m_tree->Branch("iposdist", &m_iposdist);
        m_tree->Branch("fposdist", &m_fposdist);
        m_tree->Branch("target_quality", &m_target_quality);
        m_tree->Branch("target_clippingness", &m_target_clippingness);
        m_tree->Branch("ref_quality", &m_ref_quality);
        m_tree->Branch("ref_clippingness", &m_ref_clippingness);
    }

    ~muon_performance_single_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override {
        if (m_nav->muons.empty()) return false;
        std::vector<track>::const_iterator it_target = std::find_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_targetname; });
        std::vector<track>::const_iterator it_ref = std::find_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_refname; });
        std::size_t nb_cd_muons = std::count_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == "CdClassify"; });
        std::size_t nb_wp_muons = std::count_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == "WpBasic"; });
        return it_target != m_nav->muons.end() && it_ref != m_nav->muons.end() && nb_cd_muons == 1ul && nb_wp_muons == 1ul;
    }

    bool process() override {
        if (m_nav->muons.empty()) return false;
        std::vector<track>::const_iterator it_target = std::find_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_targetname; });
        std::vector<track>::const_iterator it_ref = std::find_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_refname; });
        if (it_target == m_nav->muons.end() || it_ref == m_nav->muons.end()) return false;

        m_sec = m_nav->muons.front().ts.sec;
        m_nsec = m_nav->muons.front().ts.nsec;
        m_totq_cd = m_nav->muons.front().totq_cd;
        m_totq_wp = m_nav->muons.front().totq_wp;

        vec3 dir_target = unit(it_target->fpos - it_target->ipos);
        vec3 dir_ref = unit(it_ref->fpos - it_ref->ipos);

        vec3 mpos_target = 0.5 * (it_target->ipos + it_target->fpos);
        vec3 mpos_ref = 0.5 * (it_ref->ipos + it_ref->fpos);

        m_angle = angle(dir_target, dir_ref) * 180.0 / M_PI;
        m_distance = mag(mpos_target - mpos_ref) / 1000.0;
        m_iposdist = mag(cross(dir_ref, it_target->ipos - it_ref->ipos)) / 1000.0;
        m_fposdist = mag(cross(dir_ref, it_target->fpos - it_ref->ipos)) / 1000.0;
        m_target_quality = it_target->quality;
        m_target_clippingness = mag(cross(dir_target, -it_target->ipos)) / 1000.0;
        m_ref_quality = it_ref->quality;
        m_ref_clippingness = mag(cross(dir_ref, -it_ref->ipos)) / 1000.0;

        m_tree->Fill();
        return true;
    }

protected:

    std::shared_ptr<muon_event_navigator> m_nav;

    std::string m_targetname;
    std::string m_refname;

    TTree* m_tree;
    time_t m_sec;
    int m_nsec;
    double m_totq_cd;
    double m_totq_wp;
    double m_angle;
    double m_distance;
    double m_iposdist;
    double m_fposdist;
    double m_target_quality;
    double m_target_clippingness;
    double m_ref_quality;
    double m_ref_clippingness;

    bool save_content() override {
        m_tree->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_PERFORMANCE_MUONPERFORMANCESINGLEANALYSIS_HPP_