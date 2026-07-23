#ifndef JRAFNECK_EVENT_COSMOGENIC_HPP_
#define JRAFNECK_EVENT_COSMOGENIC_HPP_

#include "event/vertex.hpp"

struct cosmogenic {

    int run_id;
    vertex prompt;
    double dlat_mu2p;
    timestamp dt_mu2p;
    vertex delayed;
    double dlat_mu2d;
    timestamp dt_mu2d;
    timestamp dt_last_mu_with_neu;
    timestamp dt_last_mu;

    cosmogenic() = default;
    cosmogenic(const cosmogenic&) = default;
    cosmogenic(cosmogenic&&) = default;
    cosmogenic& operator=(const cosmogenic&) = default;
    cosmogenic& operator=(cosmogenic&&) = default;

    cosmogenic(int run_id_, const vertex& prompt_, const vertex& delayed_, double dlat_mu2p_, const timestamp& dt_mu2p_, double dlat_mu2d_, const timestamp& dt_mu2d_, const timestamp& dt_last_mu_with_neu_, const timestamp& dt_last_mu_) :
        run_id{run_id_},
        prompt{prompt_},
        dlat_mu2p{dlat_mu2p_},
        dt_mu2p{dt_mu2p_},
        delayed{delayed_},
        dlat_mu2d{dlat_mu2d_},
        dt_mu2d{dt_mu2d_},
        dt_last_mu_with_neu{dt_last_mu_with_neu_},
        dt_last_mu{dt_last_mu_}
    {}

};

inline bool operator<(const cosmogenic& lhs, const cosmogenic& rhs) {
    return lhs.prompt < rhs.prompt;
}

#endif // JRAFNECK_EVENT_COSMOGENIC_HPP_