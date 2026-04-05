#ifndef JRAFNECK_EVENT_IBD_HPP_
#define JRAFNECK_EVENT_IBD_HPP_

#include "event/vertex.hpp"

struct ibd {

    int run_id;
    vertex prompt;
    vertex delayed;

};

inline bool operator<(const ibd& lhs, const ibd& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_IBD_HPP_