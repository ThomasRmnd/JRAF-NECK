#ifndef JRAFNECK_ANALYSIS_MUON_LENGTH_MUONLENGTHSINGLEANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_LENGTH_MUONLENGTHSINGLEANALYSIS_HPP_

#include "analysis/muon/length/muon_length_analysis.hpp"
#include "utils/muon.hpp"

class muon_length_single_analysis : public muon_length_analysis {

public:

    using muon_length_analysis::muon_length_analysis;

    ~muon_length_single_analysis() override = default;

    bool process() override {
        if (m_run_id == 0) {
            m_run_id = m_nav->run_id;
        }
        else if (m_run_id != m_nav->run_id) {
            fill_hist(m_hist_length, m_hist_length_edges, m_hist_length_counts, m_hist_length_errors, m_hist_length_underflow, m_hist_length_overflow);
            m_tree->Fill();
            reset_run();
            m_run_id = m_nav->run_id;
        }

        if (m_nav->muons.empty()) return true;
        std::vector<track>::const_iterator it_target = std::find_if(
            m_nav->muons.begin(),
            m_nav->muons.end(),
            [&](const track& trk) { return trk.method == m_targetname; }
        );
        if (it_target == m_nav->muons.end()) return true;
        std::size_t nb_cdclassify = std::count_if(
            m_nav->muons.begin(),
            m_nav->muons.end(),
            [](const track& trk) { return trk.method == "CdClassify"; }
        );
        std::size_t nb_wpclassify = std::count_if(
            m_nav->muons.begin(),
            m_nav->muons.end(),
            [](const track& trk) { return trk.method == "WpClassify"; }
        );
        if (nb_cdclassify > 1ul || nb_wpclassify > 1ul) return true;

        m_hist_length->Fill(mag(it_target->fpos - it_target->ipos));

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_RATE_MUONRATETARGETSINGLEANALYSIS_HPP_