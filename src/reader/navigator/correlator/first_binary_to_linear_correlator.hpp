#ifndef JRAFNECK_READER_NAVIGATOR_CORRELATOR_FIRSTBINARYTOLINEARCORRELATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_CORRELATOR_FIRSTBINARYTOLINEARCORRELATOR_HPP_

#include "reader/navigator/correlator/binary_search_correlator.hpp"

class first_binary_to_linear_correlator : public binary_search_correlator {

public:

    using binary_search_correlator::binary_search_correlator;

    virtual ~first_binary_to_linear_correlator() override = default;

    virtual corrlator_results correlate(const timestamp& lo, const timestamp& hi) override {
        std::ptrdiff_t size = static_cast<std::ptrdiff_t>(m_nav->size());
        if (m_is_first_search) {
            m_is_first_search = false;
            std::cout << "[Debug]: Starting binary search\n";
            corrlator_results res = binary_search_correlator::correlate(lo, hi);
            std::cout << "[Debug]: Finished binary search\n";
            m_last_idx = res.lower;
            std::cout << "[Debug]: Get last index\n";
            m_nav->entry(m_last_idx);
            std::cout << "[Debug]: Get last timestamp (m_nav = " << m_nav << ", m_last_idx = " << m_last_idx << ")\n";
            m_last_ts = m_nav->ts();
            std::cout << "[Debug]: Returning\n";
            return res;
        }
        std::ptrdiff_t cur_idx = m_last_idx;
        timestamp cur_ts = m_last_ts;
        std::ptrdiff_t lower = cur_idx, upper = cur_idx;
        while (cur_ts < lo && cur_idx < size) {
            m_nav->entry(++cur_idx);
            cur_ts = m_nav->ts();
        }
        lower = cur_idx;
        m_last_idx = cur_idx;
        m_last_ts = cur_ts;
        while (cur_ts < hi && cur_idx < size) {
            m_nav->entry(++cur_idx);
            cur_ts = m_nav->ts();
        }
        upper = cur_idx;
        return {lower, upper};
    }

protected:

    bool m_is_first_search = true;
    std::ptrdiff_t m_last_idx = 0l;
    timestamp m_last_ts;

};

#endif // JRAFNECK_READER_NAVIGATOR_CORRELATOR_FIRSTBINARYTOLINEARCORRELATOR_HPP_