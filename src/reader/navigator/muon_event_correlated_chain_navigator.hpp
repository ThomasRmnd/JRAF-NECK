#ifndef JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_

#include "event/track.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/muon_event_user_amber_navigator.hpp"
#include "reader/navigator/muon_event_user_edwin_navigator.hpp"
#include "reader/navigator/muon_event_user_tt_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"

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
        m_edwin_nav = navigator_manager::retrieve<muon_event_user_edwin_navigator>(edwin_filepath, edwin_treename);
        if (!m_edwin_nav->is_valid()) {
            std::cerr << "Edwin chain of filepath " << edwin_filepath << " and treename " << edwin_treename << " is not valid\n";
            return;
        }
        m_tt_nav = navigator_manager::retrieve<muon_event_user_tt_navigator>(tt_filepath, tt_treename);
        if (!m_tt_nav->is_valid()) {
            std::cerr << "TT chain of filepath " << tt_filepath << " and treename " << tt_treename << " is not valid\n";
            return;
        }
    }

    virtual ~muon_event_correlated_chain_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!muon_event_navigator::entry(n)) return false;
        if (muons.empty()) return true;

        const timestamp ts  = muons.front().ts;
        const timestamp lo  = ts + timestamp{0, -1000};
        const timestamp hi  = ts + timestamp{0,  1000};

        append_correlated(m_amber_nav, lo, hi);
        append_correlated(m_edwin_nav, lo, hi);
        append_correlated(m_tt_nav,    lo, hi);

        return true;
    }

protected:

    std::shared_ptr<muon_event_user_amber_navigator> m_amber_nav;
    std::shared_ptr<muon_event_user_edwin_navigator> m_edwin_nav;
    std::shared_ptr<muon_event_user_tt_navigator> m_tt_nav;

    template<typename _Nav>
    void append_correlated(std::shared_ptr<_Nav>& nav, const timestamp& lo_ts, const timestamp& hi_ts) {
        const std::ptrdiff_t lower = lower_bound_in_navigator(nav, lo_ts);
        const std::ptrdiff_t upper = upper_bound_in_navigator(nav, hi_ts);

        for (std::ptrdiff_t i = lower; i < upper; ++i) {
            nav->entry(i);
            muons.insert(muons.end(), nav->muons.begin(), nav->muons.end());
        }
    }

    template<typename _Nav>
    std::ptrdiff_t lower_bound_in_navigator(std::shared_ptr<_Nav>& nav, const timestamp& target) {
        std::ptrdiff_t lo = 0;
        std::ptrdiff_t hi = static_cast<std::ptrdiff_t>(nav->size());

        while (lo < hi) {
            const std::ptrdiff_t mid = lo + (hi - lo) / 2;
            nav->entry(mid);
            if (nav->ts() < target) lo = mid + 1;
            else                    hi = mid;
        }
        return lo;
    }

    template<typename _Nav>
    std::ptrdiff_t upper_bound_in_navigator(std::shared_ptr<_Nav>& nav, const timestamp& target) {
        std::ptrdiff_t lo = 0;
        std::ptrdiff_t hi = static_cast<std::ptrdiff_t>(nav->size());

        while (lo < hi) {
            const std::ptrdiff_t mid = lo + (hi - lo) / 2;
            nav->entry(mid);
            if (!(target < nav->ts())) lo = mid + 1;
            else                       hi = mid;
        }
        return lo;
    }

};

#endif // JRAFNECK_READER_NAVIGATOR_MUONEVENTCORRELATEDCHAINNAVIGATOR_HPP_