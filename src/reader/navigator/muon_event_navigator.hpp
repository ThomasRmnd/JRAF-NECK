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

        m_chain->branch("sec", m_sec);
        m_chain->branch("nsec", m_nsec);

        m_chain->branch("totq_cd", m_totq_cd);
        m_chain->branch("totq_wp", m_totq_wp);

        m_chain->branch("method", m_method.ptr());
        m_chain->branch("det", m_det.ptr());
        m_chain->branch("quality", m_quality.ptr());
        m_chain->branch("iposx", m_iposx.ptr());
        m_chain->branch("iposy", m_iposy.ptr());
        m_chain->branch("iposz", m_iposz.ptr());
        m_chain->branch("fposx", m_fposx.ptr());
        m_chain->branch("fposy", m_fposy.ptr());
        m_chain->branch("fposz", m_fposz.ptr());
    }

    virtual ~muon_event_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        muons.clear();
        std::cout << "[Debug]: (3) get entry " << n << ", currently having " << muons.size() << " muons\n";
        if (m_method.empty()) return true;
        std::cout << "[Debug]: (4) get entry " << n << ", currently having " << muons.size() << " muons\n";
        muons.reserve(m_method.size());
        std::cout << "[Debug]: (5) get entry " << n << ", currently having " << muons.size() << " muons\n";
        for (std::size_t k = 0ul; k < m_method.size(); ++k) {
            muons.emplace_back(
                m_method[k], m_totq_cd, m_totq_wp,
                vec3{m_iposx[k], m_iposy[k], m_iposz[k]},
                vec3{m_fposx[k], m_fposy[k], m_fposz[k]},
                timestamp{m_sec, m_nsec},
                m_quality[k], static_cast<int>(m_det[k])
            );
        }
        std::cout << "[Debug]: (6) get entry " << n << ", currently having " << muons.size() << " muons\n";

        return true;
    }


    int run_id;
    std::vector<track> muons;

protected:

    time_t m_sec;
    int m_nsec;
    double m_totq_cd;
    double m_totq_wp;

    vector_reader<std::string> m_method;
    vector_reader<unsigned char> m_det;
    vector_reader<double> m_quality;
    vector_reader<double> m_iposx;
    vector_reader<double> m_iposy;
    vector_reader<double> m_iposz;
    vector_reader<double> m_fposx;
    vector_reader<double> m_fposy;
    vector_reader<double> m_fposz;

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTNAVIGATOR_HPP_