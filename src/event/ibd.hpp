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

struct ibd_with_muon {

    ibd i;
    timestamp dt_last_mu_with_neu;
    timestamp dt_last_mu;
    double dlat_mu2p; // to closest muon
    timestamp dt_mu2p; // to closest muon

};

inline bool operator<(const ibd_with_muon& lhs, const ibd_with_muon& rhs) {
    return lhs.i < rhs.i;
}

#endif // JRAFNECK_EVENT_IBD_HPP_