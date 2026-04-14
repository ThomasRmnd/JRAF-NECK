#ifndef JRAFNECK_READER_CHAIN_CHAINREADERMANAGER_HPP_
#define JRAFNECK_READER_CHAIN_CHAINREADERMANAGER_HPP_

#include <iostream>

#include "reader/chain/chain_reader.hpp"

class chain_reader_manager {

public:

    static std::shared_ptr<chain_reader> retrieve(const std::string& filepath, const std::string& treename) {
        std::cout << "Creating new chain for filepath " << filepath << " and treename " << treename << '\n';
        std::shared_ptr<chain_reader> chain(new chain_reader(filepath, treename));
        return chain;
    }

};

#endif // JRAFNECKREADERN_CHAINREADERMANAGER_HPP_