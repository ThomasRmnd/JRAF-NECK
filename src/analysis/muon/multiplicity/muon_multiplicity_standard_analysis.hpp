#ifndef JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYSTANDARDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYSTANDARDANALYSIS_HPP_

#include "analysis/muon/multiplicity/muon_multiplicity_analysis.hpp"

class muon_multiplicity_standard_analysis : public muon_multiplicity_analysis {

public:

    using muon_multiplicity_analysis::muon_multiplicity_analysis;

    ~muon_multiplicity_standard_analysis() override = default;

    virtual bool process() override {
        if (m_run_id == 0) {
            m_run_id = m_nav->run_id;
        }
        else if (m_run_id != m_nav->run_id) {
            fill_hist(m_hist_multiplicity, m_hist_multiplicity_edges, m_hist_multiplicity_counts, m_hist_multiplicity_errors, m_hist_multiplicity_underflow, m_hist_multiplicity_overflow);
            m_tree->Fill();
            reset_run();
            m_run_id = m_nav->run_id;
        }

        std::size_t ntrk = std::count_if(m_nav->muons.begin(), m_nav->muons.end(), [&](const track& t) { return t.method == m_targetname; });
        m_hist_multiplicity->Fill(static_cast<double>(ntrk));

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYSTANDARDANALYSIS_HPP_