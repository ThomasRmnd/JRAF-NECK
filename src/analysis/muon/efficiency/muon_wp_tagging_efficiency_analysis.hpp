#ifndef JRAFNECK_ANALYSIS_MUON_EFFICIENCY_MUONWPTAGGINGEFFICIENCYANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_EFFICIENCY_MUONWPTAGGINGEFFICIENCYANALYSIS_HPP_

#include "analysis/analysis.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"

class muon_wp_tagging_efficiency_analysis : public analysis_base {

public:

    muon_wp_tagging_efficiency_analysis(const std::string& name, const std::string& filepath, const std::string& treename) :
        analysis_base{name}
    {
        m_nav = navigator_manager::retrieve<muon_event_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        
        m_tree = new TTree("efficiency", "WP tagging efficiency");
        if (!m_tree) {
            std::cerr << "Cannot create tree efficiency\n";
            return;
        }
        m_tree->Branch("run_id", &m_run_id);
        m_tree->Branch("nb_cd_wp_30k", &m_nb_cd_wp_30k);
        m_tree->Branch("nb_cd_only_10ms", &m_nb_cd_only_10ms);
    }

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override { 
        return !m_nav->muons.empty();
    }

    bool process() override {
        if (m_run_id == 0) {
            m_run_id = m_nav->run_id;
        }
        else if (m_run_id != m_nav->run_id) {
            m_tree->Fill();
            m_run_id = m_nav->run_id;
        }

        if (m_nav->muons.empty()) return false;
        double totq_cd = m_nav->muons.front().totq_cd;
        double totq_wp = m_nav->muons.front().totq_wp;

        timestamp ts = m_nav->muons.front().ts;

        bool is_cd_only = totq_cd > 0.0 && totq_wp == 0.0;
        bool is_cd_wp = totq_cd > 0.0 && totq_wp > 0.0;

        if (is_cd_only && (ts - m_last > m_veto_window)) {
            ++m_nb_cd_only_10ms;
        }
        if (is_cd_wp && totq_cd > 30000.0) {
            ++m_nb_cd_wp_30k;
        }

        m_last = ts;

        return true;
    }

protected:

    std::shared_ptr<muon_event_navigator> m_nav;

    timestamp m_veto_window{0, 10000000};

    timestamp m_last{0l, 0};

    TTree* m_tree = nullptr;
    int m_run_id = 0;
    std::size_t m_nb_cd_wp_30k = 0ull;
    std::size_t m_nb_cd_only_10ms = 0ull;

    void reset_run() {
        m_nb_cd_wp_30k = 0ull;
        m_nb_cd_only_10ms = 0ull;
        m_last = timestamp{0l, 0};
    }

    bool save_content() override {
        if (m_run_id != 0) {
            m_tree->Fill();
        }
        m_tree->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_EFFICIENCY_MUONWPTAGGINGEFFICIENCYANALYSIS_HPP_