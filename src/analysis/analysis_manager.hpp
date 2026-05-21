#ifndef JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_
#define JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_

#include <TFile.h>

#include "analysis/analysis_registry.hpp"

class analysis_manager {

public:

    analysis_manager(const std::string& filepath, analysis_registry& reg) :
        m_filepath{filepath},
        m_reg{reg}
    {}

    bool run() {
        if (m_reg.empty()) {
            std::cout << "WARNING: Analysis registry is empty. Exiting run\n";
            return true;
        }
        
        std::cout << "\n--- Starting Analysis Loop Over " << m_reg.size() << " Data Groups ---\n";
        for (auto const& [nav, analyses] : m_reg) {
            if (!nav->is_valid()) {
                std::cerr << "\nWARNING: Navigator for this group is invalid. Skipping group\n";
                continue;
            }

            Long64_t entries = nav->size();
            std::cout << "\n[Group Start] Processing " << analyses.size() << " analyses over " << entries << " entries\n";
            
            for (Long64_t k = 0; k < entries; ++k) {
                nav->entry(k); 

                for (const std::shared_ptr<analysis_base>& analysis : analyses) {
                    if (analysis->selection()) {
                        if (!analysis->process()) return false;
                    }
                }

                if ((k + 1) % 1000 == 0) {
                     std::cout << "  [Group Status] Processed entry " << k + 1 << " / " << entries << '\n';
                }
            }

            std::cout << "[Group End] Finished processing group\n";
        }
        std::cout << "\n--- All Analysis Groups Finished ---\n";
        return true;
    }

    bool save() {
        if (m_reg.empty()) {
            std::cout << "WARNING: Analysis registry is empty. Exiting result\n";
            return true;
        }

        TFile* file = TFile::Open(m_filepath.c_str(), "RECREATE");
        if (!file) {
            std::cerr << "Cannot open file " << m_filepath << " for writing\n";
            return false;
        }

        for (auto const& [nav, analyses] : m_reg) {
            for (const auto& analysis : analyses) {
                file->cd();
                TDirectory* dir = file->mkdir(analysis->name().c_str());
                if (!dir) {
                    std::cerr << "Cannot create directory " << analysis->name() << " in file " << m_filepath << '\n';
                    return false;
                }
                std::cout << "Saving analysis " << analysis->name() << '\n';
                analysis->save(dir);
            }
        }
        file->Write();
        file->Close();

        return true;
    }

private:

    std::string m_filepath;
    analysis_registry& m_reg;

};

#endif // JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_