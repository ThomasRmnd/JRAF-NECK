#ifndef JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_

#include "event/vertex.hpp"
#include "reader/navigator/navigator.hpp"

class single_event_navigator : public navigator_base {

public:

    single_event_navigator(const std::string& filepath, const std::string& treename) :
        navigator_base{filepath, treename}
    {
        m_chain->branch("run_id", run_id);

        m_chain->branch("posx", single.pos.x);
        m_chain->branch("posy", single.pos.y);
        m_chain->branch("posz", single.pos.z);
        m_chain->branch("e", single.e);
        m_chain->branch("sec", single.ts.sec);
        m_chain->branch("nsec", single.ts.nsec);

        m_chain->branch("totq", single.totq);
        m_chain->branch("meanq", single.meanq);
        m_chain->branch("stdq", single.stdq);
        m_chain->branch("minq", single.minq);
        m_chain->branch("maxq", single.maxq);
        m_chain->branch("meant", single.meant);
        m_chain->branch("stdt", single.stdt);
        m_chain->branch("npmt", single.npmt);
        m_chain->branch("nhit", single.nhit);
        m_chain->branch("meanhit", single.meanhit);
        m_chain->branch("stdhit", single.stdhit);
    }

    virtual ~single_event_navigator() override = default;

    virtual timestamp ts() const override { return single.ts; }

    int run_id;
    vertex single;

};

#endif // JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_