#ifndef JRAFNECK_ANALYSIS_LIFETIME_DAQ_DAQTOTALTIMEANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_LIFETIME_DAQ_DAQTOTALTIMEANALYSIS_HPP_

#include <TFile.h>
#include <TTree.h>

#include "analysis/analysis.hpp"
#include "reader/navigator/daq_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "utils/timestamp.hpp"

class daq_total_time_analysis : public analysis_base {

public:

    daq_total_time_analysis(const std::string& name, const std::string& filepath, const std::string& treename) :
        analysis_base{name}
    {
        m_nav = navigator_manager::retrieve<daq_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        m_tree = new TTree("daq", "daq");
        if (!m_tree) {
            std::cerr << "Cannot create tree daq\n";
            return;
        }
        m_tree->Branch("run_id", &m_run_id);
        m_tree->Branch("start_sec", &m_start.sec);
        m_tree->Branch("start_nsec", &m_start.nsec);
        m_tree->Branch("duration_sec", &m_duration.sec);
        m_tree->Branch("duration_nsec", &m_duration.nsec);
    }

    ~daq_total_time_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override {
        return true;
    }

    bool process() override {
        if (m_run_id == 0) {
            m_run_id = m_nav->run_id;
            m_start = timestamp{m_nav->start_sec, m_nav->start_nsec};
            m_duration = timestamp{m_nav->duration_sec, m_nav->duration_nsec};
        }
        else if (m_run_id != m_nav->run_id) {
            m_tree->Fill();
            m_run_id = m_nav->run_id;
            m_start = timestamp{m_nav->start_sec, m_nav->start_nsec};
            m_duration = timestamp{m_nav->duration_sec, m_nav->duration_nsec};
        }
        else {
            m_duration += timestamp{m_nav->duration_sec, m_nav->duration_nsec};
        }
        return true;
    }

protected:

    std::shared_ptr<daq_navigator> m_nav;

    TTree* m_tree = nullptr;
    int m_run_id = 0;
    timestamp m_start{0l, 0};
    timestamp m_duration{0l, 0};

    bool save_content() override {
        m_tree->Fill();
        m_tree->Write();
        return true;
    }

};

#endif // JRAFNECK_ANALYSIS_LIFETIME_DAQ_DAQTOTALTIMEANALYSIS_HPP_