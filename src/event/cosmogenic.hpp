#ifndef JRAFNECK_EVENT_COSMOGENIC_HPP_
#define JRAFNECK_EVENT_COSMOGENIC_HPP_

#include "event/vertex.hpp"

namespace jraf {

struct cosmogenic {

    int run_id;
    jraf::vertex prompt;
    double dlat_mu2p;
    double dt_mu2p;
    jraf::vertex delayed;
    double dlat_mu2d;
    double dt_mu2d; 

};

} // namespace jraf

inline bool operator<(const jraf::cosmogenic& lhs, const jraf::cosmogenic& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_COSMOGENIC_HPP_