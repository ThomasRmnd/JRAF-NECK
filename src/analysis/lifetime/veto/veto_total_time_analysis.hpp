#ifndef JRAFNECK_ANALYSIS_LIFETIME_VETO_VETOTOTALTIMEANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LIFETIME_VETO_VETOTOTALTIMEANALYSIS_HPP_

#include <TFile.h>
#include <TTree.h>

#include "analysis/analysis.hpp"
#include "reader/navigator/veto_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "utils/veto.hpp"

class veto_total_time_analysis : public analysis_base {

public:

    veto_total_time_analysis(const std::string& name, const std::string& filepath, const std::string& treename) :
        analysis_base{name}
    {
        m_nav = navigator_manager::retrieve<veto_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        m_tree = new TTree("veto", "veto");
        if (!m_tree) {
            std::cerr << "Cannot create tree veto\n";
            return;
        }
        m_tree->Branch("run_id", &m_run_id);
        m_tree->Branch("sec", &m_veto.sec);
        m_tree->Branch("nsec", &m_veto.nsec);
        m_tree->Branch("type", &m_veto_type);
        m_tree->Branch("entries", &m_veto_entries);
    }

    ~veto_total_time_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override {
        return true;
    }

    bool process() override {
        if (m_run_id != 0 && m_run_id != m_nav->run_id) {
            for (auto& [type, veto] : m_vetoes) {
                m_veto = veto;
                m_veto_type = type;
                m_veto_entries = m_vetoes_entries[type];
                m_tree->Fill();
                veto = timestamp{0l, 0};
                m_vetoes_entries[type] = 0ul;
            }
        }

        std::unordered_map<veto_type, timestamp>::const_iterator it = g_veto_map.find(static_cast<veto_type>(m_nav->veto_type));
        if (it == g_veto_map.end()) {
            std::cerr << "Error: veto type not found in the global map\n";
            return false;
        }
        timestamp veto_duration{it->second};

        m_run_id = m_nav->run_id;
        m_vetoes[m_nav->veto_type] += timestamp{veto_duration.sec, veto_duration.nsec};
        m_vetoes_entries[m_nav->veto_type] += 1ul;
        return true;
    }


protected:

    std::shared_ptr<veto_navigator> m_nav;

    TTree* m_tree = nullptr;
    int m_run_id = 0;
    timestamp m_veto{0l, 0};
    unsigned char m_veto_type;
    std::size_t m_veto_entries;
    std::unordered_map<unsigned char, timestamp> m_vetoes;
    std::unordered_map<unsigned char, std::size_t> m_vetoes_entries;

    bool save_content() override {
        for (const auto& [type, veto] : m_vetoes) {
            m_veto = veto;
            m_veto_type = type;
            m_veto_entries = m_vetoes_entries[type];
            m_tree->Fill();
        }
        m_tree->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_LIFETIME_VETO_VETOTOTALTIMEANALYSIS_HPP_