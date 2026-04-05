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

};

inline bool operator<(const cosmogenic& lhs, const cosmogenic& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_COSMOGENIC_HPP_