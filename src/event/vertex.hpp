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

    vertex() = default;
    vertex(const vertex&) = default;
    vertex(vertex&&) = default;
    vertex& operator=(const vertex&) = default;
    vertex& operator=(vertex&&) = default;

    vertex(
        double e_, vec3 pos_, timestamp ts_, 
        double totq_, double meanq_, double stdq_, double minq_, double maxq_, 
        double meant_, double stdt_, 
        std::size_t npmt_, std::size_t nhit_, double meanhit_, double stdhit_
    ) :
        e{e_}, pos{pos_}, ts{ts_},
        totq{totq_}, meanq{meanq_}, stdq{stdq_}, minq{minq_}, maxq{maxq_},
        meant{meant_}, stdt{stdt_},
        npmt{npmt_}, nhit{nhit_}, meanhit{meanhit_}, stdhit{stdhit_}
    {}

};

inline bool operator<(const vertex& lhs, const vertex& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const vertex& v) {
    return os << '{' << v.e << ", " << v.pos << ", " << v.ts << '}';
}

#endif // JRAFNECK_EVENT_VERTEX_HPP_