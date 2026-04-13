#ifndef JRAFNECK_ANALYSIS_ANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_ANALYSIS_HPP_

#include <TDirectory.h>

#include "reader/navigator/navigator.hpp"

class analysis_base {

public:

    analysis_base(const std::string& name) :
        m_name{name}
    {}

    virtual ~analysis_base() = default;

    const std::string& name() const { return m_name; }
    virtual std::shared_ptr<navigator_base> navigator() const = 0;

    virtual bool selection() = 0;
    virtual bool process() = 0;
    virtual bool save(TDirectory* dir) {
        dir->cd();
        if (!save_content()) return false;
        dir->Write();
        return true;
    }

protected:

    std::string m_name;

    virtual bool save_content() = 0;

};

#endif // JRAFNECK_ANALYSIS_ANALYSIS_HPP_