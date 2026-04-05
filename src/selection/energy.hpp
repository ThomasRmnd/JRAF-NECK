#ifndef JRAFNECK_SELECTION_ENERGY_HPP_
#define JRAFNECK_SELECTION_ENERGY_HPP_

#include "selection/selection.hpp"

class energy_range_selection : public selection_base {

public:

    energy_range_selection(double min, double max) :
        c_min{min},
        c_max{max}
    {}

    ~energy_range_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_min <= vtx.e && vtx.e <= c_max;
    }

private:

    const double c_min;
    const double c_max;

};

#endif // JRAFNECK_SELECTION_ENERGY_HPP_