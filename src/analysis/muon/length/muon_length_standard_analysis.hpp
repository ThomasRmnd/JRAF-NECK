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

        const vec3& ipos = it_target->ipos;
        const vec3& fpos = it_target->fpos;
        vec3 dir = unit(fpos - ipos);

        double bhalf = dot(dir, ipos);
        double c = mag2(ipos) - 17700.0 * 17700.0;
        double disc = bhalf * bhalf - c;
        if (disc < 0.0) return true;
        double t1 = -bhalf - std::sqrt(disc);
        double t2 = -bhalf + std::sqrt(disc);
        double length = (t2 - t1) /  1000.0;
        // double length = mag(it_target->fpos - it_target->ipos) / 1000.0;

        m_hist_length->Fill(length);
        m_total_length += length;
        ++m_total_muon;

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_LENGTH_MUONLENGTHSTANDARDANALYSIS_HPP_