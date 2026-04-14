#ifndef JRAFNECK_READER_CHAIN_CHAINREADERMANAGER_HPP_
#define JRAFNECK_READER_CHAIN_CHAINREADERMANAGER_HPP_

#include <iostream>
#include <unordered_map>

#include "reader/chain/chain_reader.hpp"

class chain_reader_manager {

public:

    static std::shared_ptr<chain_reader> retrieve(const std::string& filepath, const std::string& treename) {
        std::string key = filepath + "::" + treename;
        // map_type::iterator it = s_cache.find(key);
        // if (it != s_cache.end()) {
        //     if (std::shared_ptr<chain_reader> existing = it->second.lock()) {
        //         std::cout << "Reusing existing chain for " << key << '\n';
        //         return existing;
        //     }
        //     s_cache.erase(it);
        // }
        std::cout << "Creating new chain for " << key << '\n';
        std::shared_ptr<chain_reader> chain(new chain_reader(filepath, treename));
        // s_cache[key] = chain;
        return chain;
    }

private:

    typedef std::unordered_map<std::string, std::weak_ptr<chain_reader>> map_type;

    inline static map_type s_cache;

};

#endif // JRAFNECKREADERN_CHAINREADERMANAGER_HPP_