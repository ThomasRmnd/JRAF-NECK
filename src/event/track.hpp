#ifndef JRAFNECK_EVENT_TRACK_HPP_
#define JRAFNECK_EVENT_TRACK_HPP_

#include "utils/timestamp.hpp"
#include "utils/vec3.hpp"

namespace jraf {

struct track {

    double totq_cd;
    double totq_wp;
    jraf::vec3 ipos;
    jraf::vec3 fpos;
    jraf::timestamp ts;

};

} // namespace jraf

inline bool operator<(const jraf::track& lhs, const jraf::track& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const jraf::track& t) {
    return os << '{' << t.totq_cd << ", " << t.totq_wp << ", " << t.ipos << ", " << t.fpos << ", " << t.ts << '}';
}

#endif // JRAFNECK_EVENT_TRACK_HPP_