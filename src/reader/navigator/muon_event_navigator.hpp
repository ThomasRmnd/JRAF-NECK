#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTNAVIGATOR_HPP_

#include "event/track.hpp"
#include "reader/containers/vector_reader.hpp"
#include "reader/navigator/navigator.hpp"

class muon_event_navigator : public navigator_base {

public:

    muon_event_navigator(const std::string& filepath, const std::string& treename) :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("run_id", run_id);

        m_chain->branch("sec", sec);
        m_chain->branch("nsec", nsec);

        m_chain->branch("totq_cd", totq_cd);
        m_chain->branch("totq_wp", totq_wp);

        m_chain->branch("method", method.ptr());
        m_chain->branch("det", det.ptr());
        m_chain->branch("quality", quality.ptr());
        m_chain->branch("iposx", iposx.ptr());
        m_chain->branch("iposy", iposy.ptr());
        m_chain->branch("iposz", iposz.ptr());
        m_chain->branch("fposx", fposx.ptr());
        m_chain->branch("fposy", fposy.ptr());
        m_chain->branch("fposz", fposz.ptr());
    }

    virtual ~muon_event_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!m_chain->entry(n)) return false;

        muons.clear();
        muons.reserve(method.size());
        for (std::size_t k = 0ul; k < method.size(); ++k) {
            muons.emplace_back(
                method[k], totq_cd, totq_wp,
                vec3{iposx[k], iposy[k], iposz[k]},
                vec3{fposx[k], fposy[k], fposz[k]},
                timestamp{sec, nsec},
                quality[k], static_cast<int>(det[k])
            );
        }

        return true;
    }


    int run_id;
    std::vector<track> muons;

protected:

    time_t sec;
    int nsec;
    double totq_cd;
    double totq_wp;

    vector_reader<std::string> method;
    vector_reader<unsigned char> det;
    vector_reader<double> quality;
    vector_reader<double> iposx;
    vector_reader<double> iposy;
    vector_reader<double> iposz;
    vector_reader<double> fposx;
    vector_reader<double> fposy;
    vector_reader<double> fposz;

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTNAVIGATOR_HPP_