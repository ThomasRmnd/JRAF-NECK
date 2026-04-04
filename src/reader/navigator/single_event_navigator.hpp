#ifndef JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_

#include "event/vertex.hpp"
#include "reader/navigator/navigator.hpp"

namespace jraf {

class single_event_navigator : public jraf::navigator_base {

public:

    single_event_navigator(const std::string& filepath, const std::string& treename) :
        jraf::navigator_base{filepath, treename}
    {
        m_chain->branch("run_id", run_id);

        m_chain->branch("posx", single.pos.x);
        m_chain->branch("posy", single.pos.y);
        m_chain->branch("posz", single.pos.z);
        m_chain->branch("e", single.e);
        m_chain->branch("sec", single.ts.sec);
        m_chain->branch("nsec", single.ts.nsec);

        m_chain->branch("totq", meta.totq);
        m_chain->branch("meanq", meta.meanq);
        m_chain->branch("stdq", meta.stdq);
        m_chain->branch("minq", meta.minq);
        m_chain->branch("maxq", meta.maxq);
        m_chain->branch("meant", meta.meant);
        m_chain->branch("stdt", meta.stdt);
        m_chain->branch("npmt", meta.npmt);
        m_chain->branch("nhit", meta.nhit);
        m_chain->branch("meanhit", meta.meanhit);
        m_chain->branch("stdhit", meta.stdhit);
    }

    int run_id;
    jraf::vertex single;
    jraf::vertex_metadata meta;

};

} // namespace jraf

#endif // JRAFNECK_READER_NAVIGATOR_SINGLEEVENTNAVIGATOR_HPP_