#ifndef JRAFNECK_READER_CHAINREADER_HPP_
#define JRAFNECK_READER_CHAINREADER_HPP_

#include <memory>
#include <string>

#include <TChain.h>

namespace jraf {

class chain_reader {

public:

    bool is_valid() const {
        return m_chain != nullptr && m_chain->GetEntries() > 0;
    }

    std::size_t size() const {
        if (!is_valid()) return 0ul;
        return static_cast<std::size_t>(m_chain->GetEntries());
    }

    bool entry(std::ptrdiff_t n) {
        if (!is_valid()) return false;
        if (n < 0 || static_cast<std::size_t>(n) >= size()) return false;
        m_chain->GetEntry(static_cast<Long64_t>(n));
        return true;
    }

    template<typename _Tp>
    int branch(const std::string& key, _Tp& value) {
        return m_chain->SetBranchAddress(key.c_str(), &value);
    }

private:

    std::shared_ptr<TChain> m_chain;

    chain_reader(const std::string& filepath, const std::string& treename) {
        m_chain = std::make_shared<TChain>(treename.c_str());
        if (m_chain) m_chain->Add(filepath.c_str());
    }

    friend class chain_reader_manager;

};

} // namespace jraf

#endif // JRAFNECK_READER_CHAINREADER_HPP_