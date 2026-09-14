#ifndef JRAFNECK_ANALYSIS_MUON_LENGTH_MUONLENGTHSTANDARDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_LENGTH_MUONLENGTHSTANDARDANALYSIS_HPP_

#include "analysis/muon/length/muon_length_analysis.hpp"

class muon_length_standard_analysis : public muon_length_analysis {

public:

    using muon_length_analysis::muon_length_analysis;

    ~muon_length_standard_analysis() override = default;

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

        std::vector<track>::const_iterator it_target = std::find_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_targetname; });
        if (it_target == m_nav->muons.end()) return true;

        double length = mag(it_target->fpos - it_target->ipos) / 1000.0;
        m_hist_length->Fill(length);
        m_total_length += length;
        ++m_total_muon;

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_RATE_MUONRATESTANDARDANALYSIS_HPP_