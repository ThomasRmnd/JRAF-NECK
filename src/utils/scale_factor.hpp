#ifndef JRAFNECK_UTILS_SCALEFACTOR_HPP_
#define JRAFNECK_UTILS_SCALEFACTOR_HPP_

#include <algorithm>
#include <fstream>
#include <vector>

#include "utils/timestamp.hpp"

namespace jraf {

class scale_factor_corrector {

public:

    struct point {

        jraf::timestamp ts;
        double factor;

        friend bool operator<(const point& lhs, const point& rhs) {
            return lhs.ts < rhs.ts;
        }

    };

    typedef std::vector<point>                              container_type;
    typedef typename container_type::size_type              size_type;
    typedef typename container_type::difference_type        difference_type;
    typedef typename container_type::const_reference        const_reference;
    typedef typename container_type::const_iterator         const_iterator;
    typedef typename container_type::const_reverse_iterator const_reverse_iterator;

    bool load(const std::string& filepath) {
        std::ifstream ifs(filepath);
        if (!ifs.is_open()) {
            std::cerr << "Cannot open file " << filepath << " for the time correction\n";
            return false;
        }
        std::string line, ts_str, factor_str;
        std::getline(ifs, line);
        while (std::getline(ifs, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            if (std::getline(ss, ts_str, ',') && std::getline(ss, factor_str, ',')) {
                long sec = std::stoll(ts_str);
                jraf::timestamp ts{static_cast<time_t>(sec), 0};
                m_data.push_back({ts, std::stod(factor_str)});
            }
        }
        std::sort(m_data.begin(), m_data.end());
        return true;
    }

    size_type size() const { return m_data.size(); }

    const_reference front() const { return m_data.front(); }
    const_reference back() const { return m_data.back(); }

    const_iterator begin() const { return m_data.begin(); }
    const_iterator cbegin() const { return m_data.cbegin(); }
    const_iterator end() const { return m_data.end(); }
    const_iterator cend() const { return m_data.cend(); }

    const_reverse_iterator rbegin() const { return m_data.rbegin(); }
    const_reverse_iterator crbegin() const { return m_data.crbegin(); }
    const_reverse_iterator rend() const { return m_data.rend(); }
    const_reverse_iterator crend() const { return m_data.crend(); }

    double interpolate(const jraf::timestamp& ts) const {
        if (m_data.empty()) return 1.0;
        std::vector<point>::const_iterator it = std::lower_bound(m_data.begin(), m_data.end(), point{ts, 0.0});
        if (it == m_data.begin()) return m_data.front().factor;
        if (it == m_data.end()) return m_data.back().factor;
        
        const point& p1 = *(it - 1);
        const point& p2 = *it;

        double fraction = jraf::timestamp_to_double(ts - p1.ts) / jraf::timestamp_to_double(p2.ts - p1.ts);
        return p1.factor + fraction * (p2.factor - p1.factor);
    }

private:

    std::vector<point> m_data;

};

class global_scale_factor_corrector {

public:

    bool load() {
        if (!m_tc_p25c.load("/cvmfs/juno.ihep.ac.cn/dbdata/main/dbdata/offline-data/Reconstruction/OMILREC/RecMap/nPEMap/Final_time_correction_P25C_AvgSPN.csv")) return false;
        if (!m_tc_p25d.load("/cvmfs/juno.ihep.ac.cn/dbdata/main/dbdata/offline-data/Reconstruction/OMILREC/RecMap/nPEMap/Final_time_correction_P25D_AvgSPN.csv")) return false;
        if (!m_tc_p25c.size() || !m_tc_p25d.size()) {
            std::cerr << "P25C or P25D time correction is empty\n";
            return false;
        }
        return true;
    }

    double interpolate(const jraf::timestamp& ts) {
        if (!m_tc_p25c.size() || !m_tc_p25d.size()) return 1.0;
        if (ts <= m_tc_p25c.back().ts) return m_tc_p25c.interpolate(ts);
        if (ts <= m_tc_p25d.back().ts) return m_tc_p25d.interpolate(ts);
        return 1.0;
    }

private:

    const int c_lower_run_id_p25c = 9789;
    const int c_upper_run_id_p25c = 11039;
    const int c_lower_run_id_p25d = 11049;
    const int c_upper_run_id_p25d = 12135;

    jraf::scale_factor_corrector m_tc_p25c;
    jraf::scale_factor_corrector m_tc_p25d;

};

} // namespace jraf

#endif // JRAFNECK_UTILS_SCALEFACTOR_HPP_