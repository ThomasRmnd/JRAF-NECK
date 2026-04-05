#ifndef JRAFNECK_EVENT_VERTEX_HPP_
#define JRAFNECK_EVENT_VERTEX_HPP_

#include "utils/timestamp.hpp"
#include "utils/vec3.hpp"

struct vertex {

    double e;
    vec3 pos;
    timestamp ts;

    double totq;
    double meanq;
    double stdq;
    double minq;
    double maxq;
    double meant;
    double stdt;
    std::size_t npmt;
    std::size_t nhit;
    double meanhit;
    double stdhit;

};

inline bool operator<(const vertex& lhs, const vertex& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const vertex& v) {
    return os << '{' << v.e << ", " << v.pos << ", " << v.ts << '}';
}

#endif // JRAFNECK_EVENT_VERTEX_HPP_