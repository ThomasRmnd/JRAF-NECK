#ifndef JRAFNECK_ANALYSIS_MUON_RATE_MUONRATEANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_RATE_MUONRATEANALYSIS_HPP_

#include <TH1D.h>

#include "analysis/analysis.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"

class muon_rate_analysis : public analysis_base {

public:

    muon_rate_analysis(const std::string& name, const std::string& filepath, const std::string& treename) :
        analysis_base{name}
    {
        m_nav = navigator_manager::retrieve<muon_event_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        m_hist_cd_only = std::make_unique<TH1D>("muon_time_difference_cd_only", "Muon time difference (CD only);#Delta t_{#mu} (s);Entries;", 50, 0.0, 2.0);
        m_hist_cd_only->SetDirectory(0);
        m_hist_wp_only = std::make_unique<TH1D>("muon_time_difference_wp_only", "Muon time difference (WP only);#Delta t_{#mu} (s);Entries;", 50, 0.0, 2.0);
        m_hist_wp_only->SetDirectory(0);
        m_hist_cd_wp = std::make_unique<TH1D>("muon_time_difference_cd_wp", "Muon time difference (CD+WP);#Delta t_{#mu} (s);Entries;", 50, 0.0, 2.0);
        m_hist_cd_wp->SetDirectory(0);
    }

    ~muon_rate_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override { 
        return !m_nav->muons.empty();
    }

    bool process() override {
        if (m_nav->muons.empty()) return false;
        double totq_cd = m_nav->muons.front().totq_cd;
        double totq_wp = m_nav->muons.front().totq_wp;

        timestamp ts = m_nav->muons.front().ts;

        bool is_cd_only = totq_cd > 0.0 && totq_wp == 0.0;
        bool is_wp_only = totq_cd == 0.0 && totq_wp > 0.0;
        bool is_cd_wp = totq_cd > 0.0 && totq_wp > 0.0;

        if (is_cd_only) {
            m_hist_cd_only->Fill(timestamp_to_double(ts - m_last_cd_only));
            m_last_cd_only = ts;
        }
        else if (is_wp_only) {
            m_hist_wp_only->Fill(timestamp_to_double(ts - m_last_wp_only));
            m_last_wp_only = ts;
        }
        else if (is_cd_wp) {
            m_hist_cd_wp->Fill(timestamp_to_double(ts - m_last_cd_wp));
            m_last_cd_wp = ts;
        }

        return true;
    }

protected:

    std::shared_ptr<muon_event_navigator> m_nav;

    std::unique_ptr<TH1D> m_hist_cd_only;
    std::unique_ptr<TH1D> m_hist_wp_only;
    std::unique_ptr<TH1D> m_hist_cd_wp;

    timestamp m_last_cd_only{0l, 0};
    timestamp m_last_wp_only{0l, 0};
    timestamp m_last_cd_wp{0l, 0};

    bool save_content() override {
        m_hist_cd_only->Write();
        m_hist_wp_only->Write();
        m_hist_cd_wp->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_RATE_MUONRATEANALYSIS_HPP_