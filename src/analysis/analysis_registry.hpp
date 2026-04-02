#ifndef JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_
#define JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_

#include <map>
#include <vector>

#include "analysis/analysis.hpp"

namespace jraf {

class analysis_registry {

public:

    typedef std::vector<std::shared_ptr<jraf::analysis_base>> vector_type;
    typedef std::map<std::shared_ptr<jraf::navigator_base>, vector_type> map_type;

    bool book(const std::shared_ptr<jraf::analysis_base>& analysis) {
        if (!analysis) {
            std::cerr << "Cannot register analysis\n";
            return false;
        }
        m_registry[analysis->navigator()].push_back(analysis);
        return true;
    }

    // bool empty TODO

    // std::size_t size TODO

    // const_iterator TODO

private:

    map_type m_registry;

    friend class analysis_manager;

};

} // namespace jraf

#endif // JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_