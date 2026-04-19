#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTUSEREDWINNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTUSEREDWINNAVIGATOR_HPP_

#include "event/track.hpp"
#include "reader/navigator/navigator.hpp"

class muon_event_user_edwin_navigator : public navigator_base {

public:

    muon_event_user_edwin_navigator(const std::string& filepath, const std::string& treename = "Edwin_Muon") :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("cd_time_s", m_cd_time_s);
        m_chain->branch("cd_time_ns", m_cd_time_ns);
        m_chain->branch("muon_classification", m_muon_classification);
        m_chain->branch("Single_enterX", m_Single_enterX);
        m_chain->branch("Single_enterY", m_Single_enterY);
        m_chain->branch("Single_enterZ", m_Single_enterZ);
        m_chain->branch("Single_exit_X", m_Single_exitX);
        m_chain->branch("Single_exit_Y", m_Single_exitY);
        m_chain->branch("Single_exit_Z", m_Single_exitZ);
        m_chain->branch("Double_enterX_1", m_Double_enterX_1);
        m_chain->branch("Double_enterY_1", m_Double_enterY_1);
        m_chain->branch("Double_enterZ_1", m_Double_enterZ_1);
        m_chain->branch("Double_exitX_1", m_Double_exitX_1);
        m_chain->branch("Double_exitY_1", m_Double_exitY_1);
        m_chain->branch("Double_exitZ_1", m_Double_exitZ_1);
        m_chain->branch("Double_enterX_2", m_Double_enterX_2);
        m_chain->branch("Double_enterY_2", m_Double_enterY_2);
        m_chain->branch("Double_enterZ_2", m_Double_enterZ_2);
        m_chain->branch("Double_exitX_2", m_Double_exitX_2);
        m_chain->branch("Double_exitY_2", m_Double_exitY_2);
        m_chain->branch("Double_exitZ_2", m_Double_exitZ_2);
    }

    virtual ~muon_event_user_edwin_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        muons.clear();
        if (m_muon_classification == 0) {
            muons.emplace_back(
                "Edwin", 0.0, 0.0,
                vec3{static_cast<double>(m_Single_enterX), static_cast<double>(m_Single_enterY), static_cast<double>(m_Single_enterZ)},
                vec3{static_cast<double>(m_Single_exitX), static_cast<double>(m_Single_exitY), static_cast<double>(m_Single_exitZ)},
                timestamp{static_cast<time_t>(m_cd_time_s), static_cast<int>(m_cd_time_ns)},
                0.0, 1
            );
        }
        else if (m_muon_classification == 1) {
            muons.emplace_back(
                "Edwin", 0.0, 0.0,
                vec3{static_cast<double>(m_Double_enterX_1), static_cast<double>(m_Double_enterY_1), static_cast<double>(m_Double_enterZ_1)},
                vec3{static_cast<double>(m_Double_exitX_1), static_cast<double>(m_Double_exitY_1), static_cast<double>(m_Double_exitZ_1)},
                timestamp{static_cast<time_t>(m_cd_time_s), static_cast<int>(m_cd_time_ns)},
                0.0, 2
            );
            muons.emplace_back(
                "Edwin", 0.0, 0.0,
                vec3{static_cast<double>(m_Double_enterX_2), static_cast<double>(m_Double_enterY_2), static_cast<double>(m_Double_enterZ_2)},
                vec3{static_cast<double>(m_Double_exitX_2), static_cast<double>(m_Double_exitY_2), static_cast<double>(m_Double_exitZ_2)},
                timestamp{static_cast<time_t>(m_cd_time_s), static_cast<int>(m_cd_time_ns)},
                0.0, 2
            );
        }

        return true;
    }

    virtual timestamp ts() const override { return timestamp{static_cast<time_t>(m_cd_time_s), static_cast<int>(m_cd_time_ns)}; }

    std::vector<track> muons;

protected:

    long long m_cd_time_s;
    long long m_cd_time_ns;
    int m_muon_classification;
    float m_Single_enterX;
    float m_Single_enterY;
    float m_Single_enterZ;
    float m_Single_exitX;
    float m_Single_exitY;
    float m_Single_exitZ;
    float m_Double_enterX_1;
    float m_Double_enterY_1;
    float m_Double_enterZ_1;
    float m_Double_exitX_1;
    float m_Double_exitY_1;
    float m_Double_exitZ_1;
    float m_Double_enterX_2;
    float m_Double_enterY_2;
    float m_Double_enterZ_2;
    float m_Double_exitX_2;
    float m_Double_exitY_2;
    float m_Double_exitZ_2;

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTUSEREDWINNAVIGATOR_HPP_