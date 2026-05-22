#ifndef JRAFNECK_READER_NAVIGATOR_CORRELATOR_BINARYSEARCHCORRELATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_CORRELATOR_BINARYSEARCHCORRELATOR_HPP_

#include "reader/navigator/correlator/correlator.hpp"

class binary_search_correlator : public correlator_base {

public:

    using correlator_base::correlator_base;

    virtual ~binary_search_correlator() override = default;

    virtual corrlator_results correlate(const timestamp& lo, const timestamp& hi) override {
        if (!m_nav->is_valid() || m_nav->size() == 0ul) return {0l, 0l};
        std::ptrdiff_t lower = lower_bound_in_navigator(lo);
        std::ptrdiff_t upper = upper_bound_in_navigator(hi);
        return {lower, upper};
    }

protected:

    std::ptrdiff_t lower_bound_in_navigator(const timestamp& target) {
        std::ptrdiff_t lo = 0;
        std::ptrdiff_t hi = static_cast<std::ptrdiff_t>(m_nav->size());

        while (lo < hi) {
            const std::ptrdiff_t mid = lo + (hi - lo) / 2;
            m_nav->entry(mid);
            if (m_nav->ts() < target) lo = mid + 1;
            else                    hi = mid;
        }
        return lo;
    }

    std::ptrdiff_t upper_bound_in_navigator(const timestamp& target) {
        std::ptrdiff_t lo = 0;
        std::ptrdiff_t hi = static_cast<std::ptrdiff_t>(m_nav->size());

        while (lo < hi) {
            const std::ptrdiff_t mid = lo + (hi - lo) / 2;
            m_nav->entry(mid);
            if (!(target < m_nav->ts())) lo = mid + 1;
            else                       hi = mid;
        }
        return lo;
    }

};

#endif // JRAFNECK_NAVIREADER_GATOR_CORRELATOR_BINARYSEARCHCORRELATOR_HPP_