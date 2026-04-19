#ifndef JRAFNECK_READER_NAVIGATOR_NAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_NAVIGATOR_HPP_

#include "reader/chain/chain_reader.hpp"
#include "reader/chain/chain_reader_manager.hpp"

class navigator_base {

public:

    navigator_base(const std::string& filepath, const std::string& treename) {
        m_chain = chain_reader_manager::retrieve(filepath, treename);
        if (!m_chain->is_valid()) {
            std::cerr << "Chain of filepath " << filepath << " and treename " << treename << " is not valid\n";
            return;
        }
    }

    virtual ~navigator_base() = default;

    bool is_valid() const { return m_chain->is_valid(); }

    std::size_t size() const { return m_chain->size(); }

    virtual bool entry(std::ptrdiff_t n) { return m_chain->entry(n); }

    virtual timestamp ts() const = 0;


protected:

    std::shared_ptr<chain_reader> m_chain;

};

#endif // JRAFNECK_READER_NAVIGATOR_NAVIGATOR_HPP_