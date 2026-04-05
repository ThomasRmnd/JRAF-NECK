#ifndef JRAFNECK_EVENT_TRACK_HPP_
#define JRAFNECK_EVENT_TRACK_HPP_

#include "utils/timestamp.hpp"
#include "utils/vec3.hpp"

struct track {

    std::string method;
    double totq_cd;
    double totq_wp;
    vec3 ipos;
    vec3 fpos;
    timestamp ts;
    double quality;
    int loc;

};

inline bool operator<(const track& lhs, const track& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const track& t) {
    return os << '{' << t.totq_cd << ", " << t.totq_wp << ", " << t.ipos << ", " << t.fpos << ", " << t.ts << '}';
}

#endif // JRAFNECK_EVENT_TRACK_HPP_