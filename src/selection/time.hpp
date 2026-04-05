#ifndef JRAFNECK_SELECTION_TIME_HPP_
#define JRAFNECK_SELECTION_TIME_HPP_

#include "selection/selection.hpp"

class time_range_selection : public selection_base {

public:

    time_range_selection(const timestamp& ref, const timestamp& min, const timestamp& max) :
        c_min{ref + min},
        c_max{ref + max}
    {}

    ~time_range_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_min <= vtx.ts && vtx.ts <= c_max;
    }

private:

    const timestamp c_min;
    const timestamp c_max;

};

#endif // JRAFNECK_SELECTION_TIME_HPP_