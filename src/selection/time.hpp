#ifndef JRAFNECK_SELECTION_TIME_HPP_
#define JRAFNECK_SELECTION_TIME_HPP_

#include "selection/selection.hpp"

namespace jraf {

class time_range_selection : public jraf::selection {

public:

    time_range_selection(const jraf::timestamp& ref, const jraf::timestamp& min, const jraf::timestamp& max) :
        c_min{ref + min},
        c_max{ref + max}
    {}

    ~time_range_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return c_min <= vtx.ts && vtx.ts <= c_max;
    }

private:

    const jraf::timestamp c_min;
    const jraf::timestamp c_max;

};

} // namespace jraf

#endif // JRAFNECK_SELECTION_TIME_HPP_