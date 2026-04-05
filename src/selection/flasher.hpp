#ifndef JRAFNECK_SELECTION_FLASHER_HPP_
#define JRAFNECK_SELECTION_FLASHER_HPP_

#include <cmath>

#include "selection/selection.hpp"

class flasher_selection : public selection_base {

public:

    flasher_selection(double stdhit_center, double stdhit_width, double stdt_center, double stdt_width) :
        c_stdhit_center{stdhit_center}, 
        c_stdhit_width{stdhit_width}, 
        c_stdt_center{stdt_center}, 
        c_stdt_width{stdt_width}
    {}

    ~flasher_selection() override = default;

    bool is_in(const vertex& v) const override {
        return std::pow((v.stdhit - c_stdhit_center) / c_stdhit_width, 2.0) + std::pow((v.stdt - c_stdt_center) / c_stdt_width, 2.0) < 1.0;
    }

private:

    const double c_stdhit_center;
    const double c_stdhit_width;
    const double c_stdt_center;
    const double c_stdt_width;

};

#endif // JRAFNECK_SELECTION_FLASHER_HPP_