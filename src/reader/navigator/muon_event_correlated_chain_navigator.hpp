#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_

#include "event/track.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/muon_event_user_amber_navigator.hpp"
#include "reader/navigator/muon_event_user_edwin_navigator.hpp"
#include "reader/navigator/muon_event_user_tt_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "reader/navigator/correlator/binary_search_correlator.hpp"
#include "reader/navigator/correlator/first_binary_to_linear_correlator.hpp"

class muon_event_correlated_chain_navigator : public muon_event_navigator {

public:

    muon_event_correlated_chain_navigator(const std::string filepath, const std::string& treename, const std::string& amber_filepath, const std::string& amber_treename, const std::string& edwin_filepath, const std::string& edwin_treename, const std::string& tt_filepath, const std::string& tt_treename) :
        muon_event_navigator{filepath, treename}
    {
        if (!is_valid()) return;

        m_amber_nav = navigator_manager::retrieve<muon_event_user_amber_navigator>(amber_filepath, amber_treename);
        if (!m_amber_nav->is_valid()) {
            std::cerr << "Amber chain of filepath " << amber_filepath << " and treename " << amber_treename << " is not valid\n";
            return;
        }
        m_amber_corr = std::make_shared<first_binary_to_linear_correlator>(m_amber_nav);
        m_edwin_nav = navigator_manager::retrieve<muon_event_user_edwin_navigator>(edwin_filepath, edwin_treename);
        if (!m_edwin_nav->is_valid()) {
            std::cerr << "Edwin chain of filepath " << edwin_filepath << " and treename " << edwin_treename << " is not valid\n";
            return;
        }
        m_edwin_corr = std::make_shared<binary_search_correlator>(m_edwin_nav);
        m_tt_nav = navigator_manager::retrieve<muon_event_user_tt_navigator>(tt_filepath, tt_treename);
        if (!m_tt_nav->is_valid()) {
            std::cerr << "TT chain of filepath " << tt_filepath << " and treename " << tt_treename << " is not valid\n";
            return;
        }
        m_tt_corr = std::make_shared<binary_search_correlator>(m_tt_nav);
    }

    virtual ~muon_event_correlated_chain_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!muon_event_navigator::entry(n)) return false;
        if (muons.empty()) return true;

        const timestamp ts  = muons.front().ts;
        const timestamp lo  = ts + timestamp{0, -1000};
        const timestamp hi  = ts + timestamp{0,  1000};

        corrlator_results res = m_amber_corr->correlate(lo, hi);
        append_correlated(m_amber_nav, res);
        res = m_edwin_corr->correlate(lo, hi);
        append_correlated(m_edwin_nav, res);
        for (const track muon : muons) {
            if (muon.method == "Tt") {
                break;
            }
        }
        res = m_tt_corr->correlate(lo, hi);
        append_correlated(m_tt_nav, res);

        return true;
    }

    virtual timestamp ts() const override { return timestamp{m_sec, m_nsec}; }

protected:

    std::shared_ptr<muon_event_user_amber_navigator> m_amber_nav;
    std::shared_ptr<correlator_base> m_amber_corr;
    std::shared_ptr<muon_event_user_edwin_navigator> m_edwin_nav;
    std::shared_ptr<correlator_base> m_edwin_corr;
    std::shared_ptr<muon_event_user_tt_navigator> m_tt_nav;
    std::shared_ptr<correlator_base> m_tt_corr;

    template<typename _Nav>
    void append_correlated(std::shared_ptr<_Nav>& nav, const corrlator_results& res) {

        for (std::ptrdiff_t i = res.lower; i < res.upper; ++i) {
            nav->entry(i);
            muons.insert(muons.end(), nav->muons.begin(), nav->muons.end());
        }
    }

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_