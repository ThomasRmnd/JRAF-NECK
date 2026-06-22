#ifndef JRAFNECK_UTILS_VETO_HPP_
#define JRAFNECK_UTILS_VETO_HPP_

#include <unordered_map>

#include "utils/timestamp.hpp"

enum class veto_type : unsigned char {
    none,
    beginning_of_job,
    missing_headers,
    big_gaps,
    muon,
    muon_cd,
    muon_wp,
};

inline std::unordered_map<veto_type, timestamp> g_veto_map {
    {veto_type::beginning_of_job, timestamp{1, 200000000}},
    {veto_type::missing_headers, timestamp{0, 7000000}},
    {veto_type::big_gaps, timestamp{1, 200000000}},
    {veto_type::muon, timestamp{0, 5000000}},
    {veto_type::muon_cd, timestamp{0, 7000000}},
    {veto_type::muon_wp, timestamp{0, 2000000}}
};

#endif // JRAFNECK_UTILS_VETO_HPP_