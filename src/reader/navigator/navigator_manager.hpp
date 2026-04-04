#ifndef JRAFNECK_READER_NAVIGATOR_NAVIGATORMANAGER_HPP_
#define JRAFNECK_READER_NAVIGATOR_NAVIGATORMANAGER_HPP_

#include "reader/navigator/navigator.hpp"

namespace jraf {

class navigator_manager {

public:

    template<typename _Nav>
    static std::shared_ptr<_Nav> retrieve(const std::string& filepath, const std::string& treename) {
        std::string key = typeid(_Nav).name();
        key += "::" + filepath + "::" + treename;

        map_type::iterator it = s_cache.find(key);
        if (it != s_cache.end()) {
            if (std::shared_ptr<navigator_base> existing = it->second.lock()) {
                if (std::shared_ptr<_Nav> typed_ptr = std::dynamic_pointer_cast<_Nav>(existing)) {
                    std::cout << "Reusing navigator " << key << '\n';
                    return typed_ptr;
                }
            }
            s_cache.erase(it);
        }

        std::cout << "Creating new Navigator for " << key << '\n';
        std::shared_ptr<_Nav> nav(new _Nav(filepath, treename));
        
        if (!nav->is_valid()) {
            std::cerr << "Failed to create and validate Navigator " << key << '\n';
            return nullptr;
        }

        s_cache[key] = nav; 
        return nav;
    }

private:

    typedef std::unordered_map<std::string, std::weak_ptr<jraf::navigator_base>> map_type;
    inline static map_type s_cache;

    navigator_manager() = delete;

};

} // namespace jraf

#endif // JRAFNECK_READER_NAVIGATOR_NAVIGATORMANAGER_HPP_