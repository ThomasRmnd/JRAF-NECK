#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERTTNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERTTNAVIGATOR_HPP_

#include <TTimeStamp.h>

#include "event/track.hpp"
#include "reader/navigator/navigator.hpp"

class muon_event_user_tt_navigator : public navigator_base {

public:

    muon_event_user_tt_navigator(const std::string& filepath, const std::string& treename = "TT") :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_chain->branch("evtID", m_evtID);
        m_chain->branch("NTotPoints", m_NTotPoints);
        m_chain->branch("PointX", m_PointX);
        m_chain->branch("PointY", m_PointY);
        m_chain->branch("PointZ", m_PointZ);
        m_chain->branch("NTracks", m_NTracks);
        m_chain->branch("NPoints", m_NPoints);
        m_chain->branch("Coeff0", m_Coeff0);
        m_chain->branch("Coeff1", m_Coeff1);
        m_chain->branch("Coeff2", m_Coeff2);
        m_chain->branch("Coeff3", m_Coeff3);
        m_chain->branch("Coeff4", m_Coeff4);
        m_chain->branch("Coeff5", m_Coeff5);
        m_chain->branch("Chi2", m_Chi2);
        m_chain->branch("start_TS", &m_start_TS);
    }

    virtual ~muon_event_user_tt_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        muons.clear();
        if (m_NTracks != 1) return true;
        std::cout << "      [Debug]: (" << n << ") Tt reconstruction has exactly 1 track\n";
        if (m_NPoints[0] < 3) return true;
        std::cout << "      [Debug]: (" << n << ") Tt reconstruction has more than 3 points\n";

        std::unordered_set<int> layers_hit;
        layers_hit.reserve(6);
        for (int i = 0; i < m_NTotPoints; ++i) {
            int lid = get_tt_layer_id(m_PointZ[i] + 26452.0);
            if (lid < 0) continue;
            layers_hit.insert(lid);
        }
        if (layers_hit.size() < 3) return true;
        std::cout << "      [Debug]: (" << n << ") Tt reconstruction has more than 3 points on 3 different layers\n";

        vec3 ipos{m_Coeff0[0], m_Coeff1[0], m_Coeff2[0] + 26452.0};
        vec3 dir = unit(vec3{m_Coeff3[0], m_Coeff4[0], m_Coeff5[0]});
        vec3 fpos = ipos - 2.0 * dot(ipos, dir) * dir;

        muons.emplace_back(
            "Tt", 0.0, 0.0,
            ipos, fpos,
            timestamp{m_start_TS->GetSec(), m_start_TS->GetNanoSec()},
            m_Chi2[0], 4
        );

        std::cout << "      [Debug]: (" << n << ") track vector size " << muons.size() << '\n';

        return true;
    }

    virtual timestamp ts() const override { return timestamp{m_start_TS->GetSec(), m_start_TS->GetNanoSec()}; }

    std::vector<track> muons;

protected:

    int m_evtID;
    int m_NTotPoints;
    float m_PointX[20];
    float m_PointY[20];
    float m_PointZ[20];
    int m_NTracks;
    int m_NPoints[20];
    double m_Coeff0[20];
    double m_Coeff1[20];
    double m_Coeff2[20];
    double m_Coeff3[20];
    double m_Coeff4[20];
    double m_Coeff5[20];
    double m_Chi2[20];
    TTimeStamp* m_start_TS = nullptr;

    int get_tt_layer_id(double z) {
        if (24000.0 <= z && z <= 25000.0) return 0;  // main
        if (25500.0 <= z && z <= 26500.0) return 1;  // main
        if (27000.0 <= z && z <= 28000.0) return 2;  // main
        if (30000.0 <= z && z <= 30200.0) return 3;  // chimney
        if (30200.0 <= z && z <= 30400.0) return 4;  // chimney
        if (30400.0 <= z && z <= 30600.0) return 5;  // chimney

        return -1; // not inside any valid layer
    }

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTUSERTTNAVIGATOR_HPP_