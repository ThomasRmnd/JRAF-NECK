#ifndef JRAFNECK_SELECTION_VERTEX_HPP_
#define JRAFNECK_SELECTION_VERTEX_HPP_

#include "selection/selection.hpp"
#include "selection/spatial.hpp"
#include "selection/time.hpp"

class vertex_selection : public selection_base {

public:

    vertex_selection(const vertex& vtx) :
        c_vtx{vtx}
    {}

    ~vertex_selection() override = default;

    virtual bool is_in(const vertex& vtx) const override = 0;

    const vertex c_vtx;

};

class vertex_correlation_selection : public vertex_selection {

public:

    vertex_correlation_selection(const vertex& vtx, double radius, const timestamp& min, const timestamp& max) :
        vertex_selection{vtx},
        c_spherical{vtx.pos, radius},
        c_time{vtx.ts, min, max}
    {}

    ~vertex_correlation_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_spherical.is_in(vtx) && c_time.is_in(vtx);
    }

private:

    const spherical_selection c_spherical;
    const time_range_selection c_time;

};

#endif // JRAFNECK_SELECTION_VERTEX_HPP_