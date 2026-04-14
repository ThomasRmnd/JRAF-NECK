#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERAMBERNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERAMBERNAVIGATOR_HPP_

#include "event/track.hpp"
#include "reader/navigator/navigator.hpp"

class muon_event_user_amber_navigator : public navigator_base {

public:

    muon_event_user_amber_navigator(const std::string& filepath, const std::string& treename = "MuonReco") :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("runID", m_runID);
        m_chain->branch("eventID", m_eventID);
        m_chain->branch("fSec", m_fSec);
        m_chain->branch("fNanoSec", m_fNanoSec);
        m_chain->branch("muonType", m_muonType);
        m_chain->branch("xin", m_xin);
        m_chain->branch("yin", m_yin);
        m_chain->branch("zin", m_zin);
        m_chain->branch("xout", m_xout);
        m_chain->branch("yout", m_yout);
        m_chain->branch("zout", m_zout);
        m_chain->branch("charge", m_charge);
    }

    virtual ~muon_event_user_amber_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        muons.clear();
        for (int k = 0; k < m_muonType + 1; ++k) {
            muons.emplace_back(
                "Amber_v5.5", 0.0, static_cast<double>(m_charge),
                vec3{static_cast<double>(m_xin), static_cast<double>(m_yin), static_cast<double>(m_zin)},
                vec3{static_cast<double>(m_xout), static_cast<double>(m_yout), static_cast<double>(m_zout)},
                timestamp{static_cast<time_t>(m_fSec), m_fNanoSec},
                0.0, 2
            );
        }

        return true;
    }

    int run_id;
    std::vector<track> muons;

protected:

    int m_runID;
    int m_eventID;
    int m_fSec;
    int m_fNanoSec;
    int m_muonType;
    float m_xin;
    float m_yin;
    float m_zin;
    float m_xout;
    float m_yout;
    float m_zout;
    float m_charge;

    timestamp ts() const { return timestamp{static_cast<time_t>(m_fSec), m_fNanoSec}; }

    friend class muon_event_correlated_chain_navigator;

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERAMBERNAVIGATOR_HPP_