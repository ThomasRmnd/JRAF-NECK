#ifndef JRAFNECK_EVENT_IBD_HPP_
#define JRAFNECK_EVENT_IBD_HPP_

#include "event/vertex.hpp"

struct ibd {

    int run_id;
    vertex prompt;
    vertex delayed;

    ibd() = default;
    ibd(const ibd&) = default;
    ibd(ibd&&) = default;
    ibd& operator=(const ibd&) = default;
    ibd& operator=(ibd&&) = default;

    ibd(int run_id_, const vertex& prompt_, const vertex& delayed_) :
        run_id{run_id_},
        prompt{prompt_},
        delayed{delayed_}
    {}

};

inline bool operator<(const ibd& lhs, const ibd& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_IBD_HPP_