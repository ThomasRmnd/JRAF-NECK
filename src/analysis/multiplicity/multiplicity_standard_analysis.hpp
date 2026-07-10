#ifndef JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYSTANDARDANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYSTANDARDANALYSIS_HPP_

#include "analysis/multiplicity/multiplicity_analysis.hpp"
#include "selection/constants.hpp"

class multiplicity_standard_analysis : public multiplicity_analysis {

public:

    using multiplicity_analysis::multiplicity_analysis;

    ~multiplicity_standard_analysis() override = default;

    bool selection() override {
        vertex event{m_nav->single};
        event.e /= m_gtc.interpolate(event.ts);

        if (!g_acrylic_sphere_cut.is_in(event)) return false;
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYSTANDARDANALYSIS_HPP_