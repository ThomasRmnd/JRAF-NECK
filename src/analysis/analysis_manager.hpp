#ifndef JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_
#define JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_

#include "analysis/analysis_registry.hpp"

namespace jraf {

class analysis_manager {

public:

    analysis_manager(jraf::analysis_registry& reg) :
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

                for (const std::shared_ptr<jraf::analysis_base>& analysis : analyses) {
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

        for (auto const& [nav, analyses] : m_reg) {
            for (const auto& analysis : analyses) {
                analysis->save();
            }
        }

        return true;
    }

private:

    jraf::analysis_registry& m_reg;

};

} // namespace jraf

#endif // JRAFNECK_ANALYSIS_ANALYSISMANAGER_HPP_