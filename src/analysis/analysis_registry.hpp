#ifndef JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_
#define JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_

#include <map>
#include <vector>

#include "analysis/analysis.hpp"

namespace jraf {

class analysis_registry {

public:

    typedef std::vector<std::shared_ptr<jraf::analysis_base>>            vector_type;
    typedef std::map<std::shared_ptr<jraf::navigator_base>, vector_type> map_type;

    typedef typename map_type::const_iterator                            const_iterator;



    bool book(const std::shared_ptr<jraf::analysis_base>& analysis) {
        if (!analysis) {
            std::cerr << "Cannot register analysis\n";
            return false;
        }
        m_registry[analysis->navigator()].push_back(analysis);
        return true;
    }

    bool empty() const { return m_registry.empty(); }

    std::size_t size() const { return m_registry.size(); }

    // const_iterator TODO
    const_iterator begin() const { return m_registry.begin(); }
    const_iterator cbegin() const { return m_registry.cbegin(); }
    const_iterator end() const { return m_registry.end(); }
    const_iterator cend() const { return m_registry.cend(); }

private:

    map_type m_registry;

};

} // namespace jraf

#endif // JRAFNECK_ANALYSIS_ANALYSISREGISTRY_HPP_