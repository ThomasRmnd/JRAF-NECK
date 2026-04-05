#ifndef JRAFNECK_EVENT_COSMOGENIC_HPP_
#define JRAFNECK_EVENT_COSMOGENIC_HPP_

#include "event/vertex.hpp"

struct cosmogenic {

    int run_id;
    vertex prompt;
    double dlat_mu2p;
    double dt_mu2p;
    vertex delayed;
    double dlat_mu2d;
    double dt_mu2d; 

    cosmogenic() = default;
    cosmogenic(const cosmogenic&) = default;
    cosmogenic(cosmogenic&&) = default;
    cosmogenic& operator=(const cosmogenic&) = default;
    cosmogenic& operator=(cosmogenic&&) = default;

    cosmogenic(int run_id_, const vertex& prompt_, const vertex& delayed_, double dlat_mu2p_, double dt_mu2p_, double dlat_mu2d_, double dt_mu2d_) :
        run_id{run_id_},
        prompt{prompt_},
        dlat_mu2p{dlat_mu2p_},
        dt_mu2p{dt_mu2p_},
        delayed{delayed_},
        dlat_mu2d{dlat_mu2d_},
        dt_mu2d{dt_mu2d_}
    {}

};

inline bool operator<(const cosmogenic& lhs, const cosmogenic& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_COSMOGENIC_HPP_