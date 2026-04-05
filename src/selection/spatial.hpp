#ifndef JRAFNECK_SELECTION_SPATIAL_HPP_
#define JRAFNECK_SELECTION_SPATIAL_HPP_

#include <limits>

#include "selection/selection.hpp"

class fiducial_volume_selection : public selection_base {

public:

    fiducial_volume_selection(double thold) :
        c_thold2{thold * thold}
    {}

    ~fiducial_volume_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return mag2(vtx.pos) <= c_thold2;
    }

private:

    const double c_thold2;


};

class height_range_selection : public selection_base {

public:

    height_range_selection(double min, double max) :
        c_min{min},
        c_max{max}
    {}

    ~height_range_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return c_min <= vtx.pos.z && vtx.pos.z <= c_max;
    }

private:

    const double c_min;
    const double c_max;

};

class radial_range_selection : public selection_base {

public:

    radial_range_selection(double min, double max) :
        c_min2{min},
        c_max2{max}
    {}

    ~radial_range_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        const double rho2 = vtx.pos.x * vtx.pos.x + vtx.pos.y * vtx.pos.y;
        return c_min2 <= rho2 && rho2 <= c_max2;
    }

private:

    const double c_min2;
    const double c_max2;

};

class chimney_selection : public selection_base {

public:

    chimney_selection(double z, double rho) :
        c_bottom{-std::numeric_limits<double>::infinity(), z},
        c_top{z, std::numeric_limits<double>::infinity()},
        c_radial{0.0, rho}
    {}

    ~chimney_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return (c_bottom.is_in(vtx) || c_top.is_in(vtx)) && c_radial.is_in(vtx);
    }

private:

    const height_range_selection c_bottom;
    const height_range_selection c_top;
    const radial_range_selection c_radial;

};

class spherical_selection : public selection_base {

public:

    spherical_selection(const vec3 center, double radius) :
        c_center{center},
        c_radius2{radius * radius}
    {}

    ~spherical_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return mag2(vtx.pos - c_center) <= c_radius2;
    }

private:

    const vec3 c_center;
    const double c_radius2;

};

class cylindrical_selection : public selection_base {

public:

    cylindrical_selection(const vec3& ipos, const vec3& fpos, double radius) :
        c_ipos{ipos},
        c_dir{unit(fpos - ipos)},
        c_radius2{radius * radius}
    {}

    ~cylindrical_selection() override = default;

    bool is_in(const vertex& vtx) const override {
        return mag2(cross(c_dir, vtx.pos - c_ipos)) <= c_radius2;
    }

private:

    const vec3 c_ipos;
    const vec3 c_dir;
    const double c_radius2;

};

#endif // JRAFNECK_SELECTION_SPATIAL_HPP_