#ifndef JRAFNECK_READER_NAVIGATOR_CORRELATOR_CORRELATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_CORRELATOR_CORRELATOR_HPP_

#include "reader/navigator/navigator.hpp"

struct corrlator_results {
    std::ptrdiff_t lower;
    std::ptrdiff_t upper;
};

class correlator_base {

public:

    correlator_base(const std::shared_ptr<navigator_base>& nav) :
        m_nav{nav}
    {}

    virtual ~correlator_base() = default;

    virtual corrlator_results correlate(const timestamp& lo, const timestamp& hi) = 0;

protected:

    std::shared_ptr<navigator_base> m_nav;

};

#endif // JRAFNECK_READER_NAVIGATOR_CORRELATOR_CORRELATOR_HPP_