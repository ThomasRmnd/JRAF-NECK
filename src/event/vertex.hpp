#ifndef JRAFNECK_EVENT_VERTEX_HPP_
#define JRAFNECK_EVENT_VERTEX_HPP_

#include "utils/timestamp.hpp"
#include "utils/vec3.hpp"

namespace jraf {

struct vertex {

    double e;
    jraf::vec3 pos;
    jraf::timestamp ts;

};

struct vertex_metadata {

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

} // namespace jraf

inline bool operator<(const jraf::vertex& lhs, const jraf::vertex& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const jraf::vertex& v) {
    return os << '{' << v.e << ", " << v.pos << ", " << v.ts << '}';
}

#endif // JRAFNECK_EVENT_VERTEX_HPP_