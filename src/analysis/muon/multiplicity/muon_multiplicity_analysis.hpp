#ifndef JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYANALYSIS_HPP_
#define JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYANALYSIS_HPP_

#include <TH1D.h>

#include "analysis/analysis.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"

class muon_multiplicity_analysis : public analysis_base {

public:

    muon_multiplicity_analysis(const std::string& name, const std::string& filepath, const std::string& treename, const std::string& targetname) :
        analysis_base{name},
        m_targetname{targetname}
    {
        m_nav = navigator_manager::retrieve<muon_event_navigator>(filepath, treename);
        if (!m_nav->is_valid()) {
            std::cerr << "Cannot retrieve navigator of filepath " << filepath << " and treename " << treename << '\n';
            return;
        }
        m_hist_multiplicity = std::make_unique<TH1D>("muon_multiplicity", "Muon multiplicity;n_{trk};Entries;", 10, 1.0, 11.0);
        m_hist_multiplicity->SetDirectory(0);

        m_tree = new TTree("multiplicity", "Muon multiplicity");
        if (!m_tree) {
            std::cerr << "Cannot create tree length\n";
            return;
        }
        m_tree->Branch("run_id", &m_run_id);
        m_tree->Branch("hist_length_edges", &m_hist_multiplicity_edges);
        m_tree->Branch("hist_length_counts", &m_hist_multiplicity_counts);
        m_tree->Branch("hist_length_errors", &m_hist_multiplicity_errors);
        m_tree->Branch("hist_length_underflow", &m_hist_multiplicity_underflow);
        m_tree->Branch("hist_length_overflow", &m_hist_multiplicity_overflow);
    }

    virtual ~muon_multiplicity_analysis() override = default;

    std::shared_ptr<navigator_base> navigator() const override {
        return m_nav;
    }

    bool selection() override { 
        return !m_nav->muons.empty();
    }

    virtual bool process() override = 0; 

protected:

    std::shared_ptr<muon_event_navigator> m_nav;

    std::string m_targetname;

    std::unique_ptr<TH1D> m_hist_multiplicity;

    TTree* m_tree = nullptr;
    int m_run_id = 0;
    std::vector<double> m_hist_multiplicity_edges;
    std::vector<double> m_hist_multiplicity_counts;
    std::vector<double> m_hist_multiplicity_errors;
    double m_hist_multiplicity_underflow;
    double m_hist_multiplicity_overflow;

    void reset_run() {
        m_hist_multiplicity->Reset();
    }

    bool save_content() override {
        if (m_run_id != 0) {
            fill_hist(m_hist_multiplicity, m_hist_multiplicity_edges, m_hist_multiplicity_counts, m_hist_multiplicity_errors, m_hist_multiplicity_underflow, m_hist_multiplicity_overflow);
            m_tree->Fill();
        }
        m_tree->Write();
        return true;
    }

    void fill_hist(const std::unique_ptr<TH1D>& h, std::vector<double>& edges, std::vector<double>& counts, std::vector<double>& errors, double& underflow, double& overflow) {
        edges.clear();
        counts.clear();
        errors.clear();

        const int nbins = h->GetNbinsX();

        edges.reserve(nbins + 1);
        counts.reserve(nbins);
        errors.reserve(nbins);

        for (int i = 1; i <= nbins; ++i) {
            edges.push_back(h->GetBinLowEdge(i));
            counts.push_back(h->GetBinContent(i));
            errors.push_back(h->GetBinError(i));
        }

        edges.push_back(
            h->GetBinLowEdge(nbins) + h->GetBinWidth(nbins)
        );

        underflow = h->GetBinContent(0);
        overflow = h->GetBinContent(h->GetNbinsX() + 1);
    }

};

#endif // JRAFNECK_ANALYSIS_MUON_MULTIPLICITY_MUONMULTIPLICITYANALYSIS_HPP_