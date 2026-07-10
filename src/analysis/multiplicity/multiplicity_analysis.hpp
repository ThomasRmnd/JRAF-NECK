#ifndef JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYANALYSIS_HPP_

#include <set>

#include <TTree.h>

#include "analysis/analysis.hpp"
#include "event/single.hpp"
#include "reader/navigator/single_event_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "utils/scale_factor.hpp"

class multiplicity_analysis : public analysis_base {

public:

    multiplicity_analysis(const std::string& name, const std::string& filepath, const std::string& treename) :
        analysis_base{name}
    {
        m_nav = navigator_manager::retrieve<single_event_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Canoot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        if (!m_gtc.load()) {
            std::cerr << "Cannot load global time corrector\n";
            return;
        }
    }

    virtual ~multiplicity_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    virtual bool selection() override = 0;

    bool process() override {
        m_multiplicities.insert({m_nav->run_id, m_nav->single});
        return true;
    }

protected:

    std::shared_ptr<single_event_navigator> m_nav;

    global_scale_factor_corrector m_gtc;

    std::set<single> m_multiplicities;

    bool save_content() override {
        TTree* t = new TTree("events", "Events");
        if (!t) {
            std::cerr << "Cannot create tree events\n";
            return false;
        }
        int run_id;
        vec3 pos;
        timestamp ts;
        double e;
        t->Branch("run_id", &run_id);
        t->Branch("posx", &pos.x);
        t->Branch("posy", &pos.y);
        t->Branch("posz", &pos.z);
        t->Branch("sec", &ts.sec);
        t->Branch("nsec", &ts.nsec);
        t->Branch("e", &e);

        for (std::set<single>::const_iterator it = m_multiplicities.begin(); it != m_multiplicities.end(); ++it) {
            run_id = it->run_id;
            pos = it->event.pos;
            ts = it->event.ts;
            e = it->event.e / m_gtc.interpolate(it->event.ts);
            t->Fill();
        }

        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_MULTIPLICITY_MULTIPLICITYANALYSIS_HPP_