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

    track() = default;
    track(const track&) = default;
    track(track&&) = default;
    track& operator=(const track&) = default;
    track& operator=(track&&) = default;

    track(
        std::string method_, double totq_cd_, double totq_wp_, 
        vec3 ipos_, vec3 fpos_, timestamp ts_, 
        double quality_, int loc_
    ) :
        method{method_}, totq_cd{totq_cd_}, totq_wp{totq_wp_}, 
        ipos{ipos_}, fpos{fpos_}, ts{ts_}, 
        quality{quality_}, loc{loc_}
    {}

};

inline bool operator<(const track& lhs, const track& rhs) {
    return lhs.ts < rhs.ts;
}

inline std::ostream& operator<<(std::ostream& os, const track& t) {
    return os << '{' << t.totq_cd << ", " << t.totq_wp << ", " << t.ipos << ", " << t.fpos << ", " << t.ts << '}';
}

#endif // JRAFNECK_EVENT_TRACK_HPP_