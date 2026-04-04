#ifndef JRAFNECK_UTILS_MUONLOOKUP_HPP_
#define JRAFNECK_UTILS_MUONLOOKUP_HPP_

#include "reader/navigator/ibd_like_event_navigator.hpp"
#include "utils/timestamp.hpp"

namespace jraf {

class is_cd_muon_lookup {

public:

    void fill(const std::shared_ptr<jraf::ibd_like_event_navigator>& nav) {
        m_times.clear();
        for (std::size_t k = 0ul; k < nav->method_mu.size(); ++k) {
            if (nav->totq_cd_mu[k] <= 0.0) continue;
            m_times.push_back(jraf::timestamp{nav->sec_mu[k], nav->nsec_mu[k]});
        }
        std::sort(m_times.begin(), m_times.end());
    }

    bool operator[](const jraf::timestamp& ts) const {
        if (m_times.empty()) return false;
        jraf::timestamp low_bound = ts - window;
        jraf::timestamp high_bound = ts + window;
        std::vector<jraf::timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<jraf::timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high) > 0l;
    }

private:

    std::vector<jraf::timestamp> m_times;
    const jraf::timestamp window{0, 1000};

};

class multiplicity_muon_lookup {

public:

    void fill(const std::shared_ptr<jraf::ibd_like_event_navigator>& nav, const std::string& target) {
        m_times.clear();
        for (std::size_t k = 0ul; k < nav->method_mu.size(); ++k) {
            if (nav->method_mu[k] != target) continue;
            m_times.push_back(jraf::timestamp{nav->sec_mu[k], nav->nsec_mu[k]});
        }
        std::sort(m_times.begin(), m_times.end());
    }

    std::size_t operator[](const jraf::timestamp& ts) const {
        if (m_times.empty()) return 0ul;
        jraf::timestamp low_bound = ts - window;
        jraf::timestamp high_bound = ts + window;
        std::vector<jraf::timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<jraf::timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high);
    }

private:

    std::vector<jraf::timestamp> m_times;
    const jraf::timestamp window{0, 1000};

};

class stopping_muon_lookup {

public:

    void fill(const std::shared_ptr<jraf::ibd_like_event_navigator>& nav, const std::string& target) {
        m_times.clear();
        const jraf::vec3 dummy_point{-999999.0, -999999.0, -999999.0};
        for (std::size_t k = 0ul; k < nav->method_mu.size(); ++k) {
            if (nav->method_mu[k] != target) continue;
            jraf::vec3 ipos{nav->posx_mu[k], nav->posy_mu[k], nav->posz_mu[k]};
            jraf::vec3 dir = unit(jraf::vec3{nav->dirx_mu[k], nav->diry_mu[k], nav->dirz_mu[k]});
            jraf::vec3 to_dummy = unit(dummy_point - ipos);
            double alpha = angle(dir, to_dummy);
            if (alpha > epsilon) continue;
            m_times.push_back(jraf::timestamp{nav->sec_mu[k], nav->nsec_mu[k]});
        }
        std::sort(m_times.begin(), m_times.end());
    }
        
    bool operator[](const jraf::timestamp& ts) const {
        if (m_times.empty()) return false;
        jraf::timestamp low_bound = ts - window;
        jraf::timestamp high_bound = ts + window;
        std::vector<jraf::timestamp>::const_iterator it_low = std::lower_bound(m_times.begin(), m_times.end(), low_bound);
        std::vector<jraf::timestamp>::const_iterator it_high = std::upper_bound(m_times.begin(), m_times.end(), high_bound);
        return std::distance(it_low, it_high) > 0l;
    }

private:

    std::vector<jraf::timestamp> m_times;
    const jraf::timestamp window{0, 1000};
    const double epsilon = 1e-4;

};

} // namespace jraf

#endif // JRAFNECK_UTILS_MUONLOOKUP_HPP_