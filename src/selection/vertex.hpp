#ifndef JRAFNECK_SELECTION_VERTEX_HPP_
#define JRAFNECK_SELECTION_VERTEX_HPP_

#include "selection/selection.hpp"
#include "selection/spatial.hpp"
#include "selection/time.hpp"

namespace jraf {

class vertex_selection : public jraf::selection {

public:

    vertex_selection(const jraf::vertex& vtx) :
        c_vtx{vtx}
    {}

    ~vertex_selection() override = default;

    virtual bool is_in(const jraf::vertex& vtx) const = 0;

    const jraf::vertex c_vtx;

};

class vertex_correlation_selection : public vertex_selection {

public:

    vertex_correlation_selection(const jraf::vertex& vtx, double radius, const jraf::timestamp& min, const jraf::timestamp& max) :
        vertex_selection{vtx},
        c_spherical{vtx.pos, radius},
        c_time{vtx.ts, min, max}
    {}

    ~vertex_correlation_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const {
        return c_spherical.is_in(vtx) && c_time.is_in(vtx);
    }

private:

    const jraf::spherical_selection c_spherical;
    const jraf::time_range_selection c_time;

};

} // namespace jraf

#endif // JRAFNECK_SELECTION_VERTEX_HPP_