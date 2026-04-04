#ifndef JRAFNECK_ANALYSIS_ANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_ANALYSIS_HPP_

#include "reader/navigator/navigator.hpp"

namespace jraf {

class analysis_base {

public:

    analysis_base(const std::string& name) :
        m_name{name}
    {}

    virtual ~analysis_base() = default;

    virtual std::shared_ptr<jraf::navigator_base> navigator() const = 0;

    virtual bool selection() = 0;
    virtual bool process() = 0;
    virtual bool save() = 0;

protected:

    std::string m_name;

};

} // namespace jraf

#endif // JRAFNECK_ANALYSIS_ANALYSIS_HPP_