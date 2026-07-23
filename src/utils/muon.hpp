#ifndef JRAFNECK_UTILS_MUON_HPP_
#define JRAFNECK_UTILS_MUON_HPP_

#include "event/track.hpp"

class is_cd_muon_lookup {

public:

    void fill(const std::vector<track>& muons) {
        m_times.clear();
        for (const track& muon : muons) {
            if (muon.totq_cd <= 0.0) continue;
            m_times.push_back(muon.ts);
        }
        std::sort(m_times.begin(), m_times.end());
    }

    bool operator[](const timestamp& ts) const {
        if (m_times.empty()) return false;
        timestamp low_bound = ts - window;
        timestamp high_bound = ts + window;
        std::vector<timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high) > 0l;
    }

private:

    std::vector<timestamp> m_times;
    const timestamp window{0, 1000};

};

class multiplicity_muon_lookup {

public:

    void fill(const std::vector<track>& muons, const std::string& target) {
        m_times.clear();
        for (const track& muon : muons) {
            if (muon.method != target) continue;
            m_times.push_back(muon.ts);
        }
        std::sort(m_times.begin(), m_times.end());
    }

    std::size_t operator[](const timestamp& ts) const {
        if (m_times.empty()) return 0ul;
        timestamp low_bound = ts - window;
        timestamp high_bound = ts + window;
        std::vector<timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high);
    }

private:

    std::vector<timestamp> m_times;
    const timestamp window{0, 1000};

};

class stopping_muon_lookup {

public:

    void fill(const std::vector<track>& muons, const std::string& target) {
        m_times.clear();
        for (const track& muon : muons) {
            if (muon.method != target) continue;
            if (mag2(muon.fpos) < 40000.0 * 40000.0) continue; // not stopping
            m_times.push_back(muon.ts);
        }
        std::sort(m_times.begin(), m_times.end());
    }
        
    bool operator[](const timestamp& ts) const {
        if (m_times.empty()) return false;
        timestamp low_bound = ts - window;
        timestamp high_bound = ts + window;
        std::vector<timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high) > 0l;
    }

private:

    std::vector<timestamp> m_times;
    const timestamp window{0, 1000};

};

struct dt_to_last_muon_result {
    bool is_set = false;
    timestamp dt_last_mu{-1, 0};
};

inline dt_to_last_muon_result calculate_dt_to_last_muon(const vertex& prompt, const std::vector<track>& muons) {
    dt_to_last_muon_result res;
    is_cd_muon_lookup cd_muons_in_event;
    cd_muons_in_event.fill(muons);
    for (const track& muon : muons) {
        if (prompt.ts < muon.ts) continue;
        if (!cd_muons_in_event[muon.ts]) continue;
        if (res.is_set && prompt.ts - muon.ts > res.dt_last_mu) continue;
        res.dt_last_mu = prompt.ts - muon.ts;
        res.is_set = true;
    }
    return res;
}

inline dt_to_last_muon_result calculate_dt_to_last_muon_with_neutron(const vertex& prompt, const std::vector<track>& muons, const std::vector<vertex>& neutrons) {
    dt_to_last_muon_result res;
    is_cd_muon_lookup cd_muons_in_event;
    cd_muons_in_event.fill(muons);
    for (const track& muon : muons) {
        if (prompt.ts < muon.ts) continue;
        if (!cd_muons_in_event[muon.ts]) continue;
        bool found_neutron = false;
        for (const vertex& neutron : neutrons) {
            if (neutron.ts < muon.ts + timestamp{0, 20000} || muon.ts + timestamp{0, 2000000} < neutron.ts) continue;
            if (!g_acrylic_sphere_cut.is_in(neutron)) continue;
            found_neutron = true;
            break;
        }
        if (!found_neutron) continue;
        if (res.is_set && prompt.ts - muon.ts > res.dt_last_mu) continue;
        res.dt_last_mu = prompt.ts - muon.ts;
        res.is_set = true;
    }
    return res;
}

#endif // JRAFNECK_UTILS_MUON_HPP_