#ifndef JRAFNECK_EVENT_SINGLE_HPP_
#define JRAFNECK_EVENT_SINGLE_HPP_

#include "event/vertex.hpp"

struct single {

    int run_id;
    vertex event;

    single() = default;
    single(const single&) = default;
    single(single&&) = default;
    single& operator=(const single&) = default;
    single& operator=(single&&) = default;

    single(int run_id_, const vertex& event_) :
        run_id{run_id_},
        event{event_}
    {}

};

inline bool operator<(const single& lhs, const single& rhs) {
    return lhs.event < rhs.event;
}

#endif // JRAFNECK_EVENT_SINGLE_HPP_