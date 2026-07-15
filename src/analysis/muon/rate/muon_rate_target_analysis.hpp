#ifndef JRAFNECK_ANALYSIS_MUON_RATE_MUONRATETARGETANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_RATE_MUONRATETARGETANALYSIS_HPP_

#include "analysis/muon/rate/muon_rate_analysis.hpp"

class muon_rate_target_analysis : public muon_rate_analysis {

public:

    muon_rate_target_analysis(const std::string& name, const std::string& filepath, const std::string& treename, const std::string& target) :
        muon_rate_analysis{name, filepath, treename},
        m_target{target}
    {}

    ~muon_rate_target_analysis() override = default;

    bool process() override {
        if (m_run_id == 0) {
            m_run_id = m_nav->run_id;
        }
        else if (m_run_id != m_nav->run_id) {
            fill_hist(m_hist_cd_only, m_hist_cd_only_edges, m_hist_cd_only_counts, m_hist_cd_only_errors, m_hist_cd_only_underflow, m_hist_cd_only_overflow);
            fill_hist(m_hist_wp_only, m_hist_wp_only_edges, m_hist_wp_only_counts, m_hist_wp_only_errors, m_hist_wp_only_underflow, m_hist_wp_only_overflow);
            fill_hist(m_hist_cd_wp, m_hist_cd_wp_edges, m_hist_cd_wp_counts, m_hist_cd_wp_errors, m_hist_cd_wp_underflow, m_hist_cd_wp_overflow);
            m_tree->Fill();
            reset_run();
            m_run_id = m_nav->run_id;
        }

        if (m_nav->muons.empty()) return false;
        std::vector<track>::const_iterator it = std::find_if(
            m_nav->muons.begin(),
            m_nav->muons.end(),
            [&](const track& trk) { return trk.method == m_target; }
        );
        if (it == m_nav->muons.end()) return false;

        double totq_cd = it->totq_cd;
        double totq_wp = it->totq_wp;

        timestamp ts = it->ts;

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

    std::string m_target;

};

#endif // JRAFNECK_ANALYSIS_MUON_RATE_MUONRATETARGETANALYSIS_HPP_