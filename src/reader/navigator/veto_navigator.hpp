#ifndef JRAFNECK_READER_NAVIGATOR_VETONAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_VETONAVIGATOR_HPP_

#include "reader/navigator/navigator.hpp"

class veto_navigator : public navigator_base {

public:

    veto_navigator(const std::string& filepath, const std::string& treename) :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("run_id", run_id);

        m_chain->branch("sec", sec);
        m_chain->branch("nsec", nsec);

        m_chain->branch("veto_type", veto_type);
    }

    virtual ~veto_navigator() override = default;

    virtual timestamp ts() const override { return timestamp{sec, nsec}; }

    int run_id;

    time_t sec;
    int nsec;

    unsigned char veto_type;

};

#endif // JRAFNECK_READER_NAVIGATOR_VETONAVIGATOR_HPP_