#ifndef JRAFNECK_SELECTION_SPATIAL_HPP_
#define JRAFNECK_SELECTION_SPATIAL_HPP_

#include <limits>

#include "selection/selection.hpp"

namespace jraf {

class fiducial_volume_selection : public jraf::selection {

public:

    fiducial_volume_selection(double thold) :
        c_thold2{thold * thold}
    {}

    ~fiducial_volume_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return mag2(vtx.pos) <= c_thold2;
    }

private:

    const double c_thold2;


};

class height_range_selection : public jraf::selection {

public:

    height_range_selection(double min, double max) :
        c_min{min},
        c_max{max}
    {}

    ~height_range_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return c_min <= vtx.pos.z && vtx.pos.z <= c_max;
    }

private:

    const double c_min;
    const double c_max;

};

class radial_range_selection : public jraf::selection {

public:

    radial_range_selection(double min, double max) :
        c_min2{min},
        c_max2{max}
    {}

    ~radial_range_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        const double rho2 = vtx.pos.x * vtx.pos.x + vtx.pos.y * vtx.pos.y;
        return c_min2 <= rho2 && rho2 <= c_max2;
    }

private:

    const double c_min2;
    const double c_max2;

};

class chimney_selection : public jraf::selection {

public:

    chimney_selection(double z, double rho) :
        c_bottom{-std::numeric_limits<double>::infinity(), z},
        c_top{z, std::numeric_limits<double>::infinity()},
        c_radial{0.0, rho}
    {}

    ~chimney_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return c_bottom.is_in(vtx) && c_top.is_in(vtx) && c_radial.is_in(vtx);
    }

private:

    const jraf::height_range_selection c_bottom;
    const jraf::height_range_selection c_top;
    const jraf::radial_range_selection c_radial;

};

class spherical_selection : public jraf::selection {

public:

    spherical_selection(const jraf::vec3 center, double radius) :
        c_center{center},
        c_radius2{radius * radius}
    {}

    ~spherical_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return mag2(vtx.pos - c_center) <= c_radius2;
    }

private:

    const jraf::vec3 c_center;
    const double c_radius2;

};

class cylindrical_selection : public jraf::selection {

public:

    cylindrical_selection(const jraf::vec3& ipos, const jraf::vec3& fpos, double radius) :
        c_ipos{ipos},
        c_dir{unit(fpos - ipos)},
        c_radius2{radius * radius}
    {}

    ~cylindrical_selection() override = default;

    bool is_in(const jraf::vertex& vtx) const override {
        return mag2(cross(c_dir, vtx.pos - c_ipos)) <= c_radius2;
    }

private:

    const jraf::vec3 c_ipos;
    const jraf::vec3 c_dir;
    const double c_radius2;

};

} // namespace jraf

#endif // JRAFNECK_SELECTION_SPATIAL_HPP_