#ifndef JRAFNECK_READER_NAVIGATOR_DAQNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_DAQNAVIGATOR_HPP_

#include "reader/navigator/navigator.hpp"

class daq_navigator : public navigator_base {

public:

    daq_navigator(const std::string& filepath, const std::string& treename) :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("run_id", run_id);

        m_chain->branch("start_sec", start_sec);
        m_chain->branch("start_nsec", start_nsec);

        m_chain->branch("duration_sec", duration_sec);
        m_chain->branch("duration_nsec", duration_nsec);
    }

    virtual ~daq_navigator() override = default;

    int run_id;

    time_t start_sec;
    int start_nsec;

    time_t duration_sec;
    int duration_nsec;

};

#endif // JRAFNECK_READER_NAVIGATOR_DAQNAVIGATOR_HPP_