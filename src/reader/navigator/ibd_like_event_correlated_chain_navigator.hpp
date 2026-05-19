#ifndef JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_

#include "event/track.hpp"
#include "event/vertex.hpp"
// #include "reader/navigator/muon_event_correlated_chain_navigator.hpp"
#include "reader/navigator/muon_event_navigator.hpp"
#include "reader/navigator/navigator_manager.hpp"
#include "reader/navigator/single_event_navigator.hpp"
#include "reader/navigator/correlator/binary_search_correlator.hpp"
#include "reader/navigator/correlator/first_binary_to_linear_correlator.hpp"

class ibd_like_event_correlated_chain_navigator : public navigator_base {

public:

    ibd_like_event_correlated_chain_navigator(const std::string& filepath, const std::string& treename, const std::string& neutron_filepath, const std::string& neutron_treename, const std::string& multiplicity_filepath, const std::string& multiplicity_treename, const std::string& muon_filepath, const std::string& muon_treename) :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

        m_neu_nav = navigator_manager::retrieve<single_event_navigator>(neutron_filepath, neutron_treename);
        if (!m_neu_nav->is_valid()) {
            std::cerr << "Neutron navigator of filepath " << neutron_filepath << " and treename " << neutron_treename << " is not valid\n";
            return;
        }
        m_neu_corr = std::make_shared<first_binary_to_linear_correlator>(m_neu_nav);

        m_mult_nav = navigator_manager::retrieve<single_event_navigator>(multiplicity_filepath, multiplicity_treename);
        if (!m_mult_nav->is_valid()) {
            std::cerr << "Multiplicity navigator of filepath " << multiplicity_filepath << " and treename " << multiplicity_treename << " is not valid\n";
            return;
        }
        m_mult_corr = std::make_shared<first_binary_to_linear_correlator>(m_mult_nav);

        m_muon_nav = navigator_manager::retrieve<muon_event_navigator>(muon_filepath, muon_treename);
        if (!m_muon_nav->is_valid()) {
            std::cerr << "Muon navigator of filepath " << muon_filepath << " and treename " << muon_treename << " is not valid\n";
            return;
        }
        m_muon_corr = std::make_shared<first_binary_to_linear_correlator>(m_muon_nav);

        m_chain->branch("run_id", run_id);

        m_chain->branch("posx_p", prompt.pos.x);
        m_chain->branch("posy_p", prompt.pos.y);
        m_chain->branch("posz_p", prompt.pos.z);
        m_chain->branch("e_p", prompt.e);
        m_chain->branch("sec_p", prompt.ts.sec);
        m_chain->branch("nsec_p", prompt.ts.nsec);

        m_chain->branch("totq_p", prompt.totq);
        m_chain->branch("meanq_p", prompt.meanq);
        m_chain->branch("stdq_p", prompt.stdq);
        m_chain->branch("minq_p", prompt.minq);
        m_chain->branch("maxq_p", prompt.maxq);
        m_chain->branch("meant_p", prompt.meant);
        m_chain->branch("stdt_p", prompt.stdt);
        m_chain->branch("npmt_p", prompt.npmt);
        m_chain->branch("nhit_p", prompt.nhit);
        m_chain->branch("meanhit_p", prompt.meanhit);
        m_chain->branch("stdhit_p", prompt.stdhit);

        m_chain->branch("posx_d", delayed.pos.x);
        m_chain->branch("posy_d", delayed.pos.y);
        m_chain->branch("posz_d", delayed.pos.z);
        m_chain->branch("e_d", delayed.e);
        m_chain->branch("sec_d", delayed.ts.sec);
        m_chain->branch("nsec_d", delayed.ts.nsec);

        m_chain->branch("totq_d", delayed.totq);
        m_chain->branch("meanq_d", delayed.meanq);
        m_chain->branch("stdq_d", delayed.stdq);
        m_chain->branch("minq_d", delayed.minq);
        m_chain->branch("maxq_d", delayed.maxq);
        m_chain->branch("meant_d", delayed.meant);
        m_chain->branch("stdt_d", delayed.stdt);
        m_chain->branch("npmt_d", delayed.npmt);
        m_chain->branch("nhit_d", delayed.nhit);
        m_chain->branch("meanhit_d", delayed.meanhit);
        m_chain->branch("stdhit_d", delayed.stdhit);
    }

    virtual ~ibd_like_event_correlated_chain_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        const timestamp ts = prompt.ts;
        const timestamp lo = ts + timestamp{0, -1000};
        const timestamp hi = ts + timestamp{0,  1000};

        neutrons.clear();
        corrlator_results res = m_neu_corr->correlate(lo, hi);
        std::cout << "[Debug] Correlated neutrons for event " << n << ": " << res.lower << " to " << res.upper << '\n';
        append_correlated_vertex(m_neu_nav, res, neutrons);
        multiplicities.clear();
        res = m_mult_corr->correlate(lo, hi);
        std::cout << "[Debug] Correlated multiplicities for event " << n << ": " << res.lower << " to " << res.upper << '\n';
        append_correlated_vertex(m_mult_nav, res, multiplicities);
        res = m_muon_corr->correlate(lo, hi);
        muons.clear();
        std::cout << "[Debug] Correlated muons for event " << n << ": " << res.lower << " to " << res.upper << '\n';
        append_correlated_track(m_muon_nav, res);

        std::cout << "[Debug] Event " << n << ": " << neutrons.size() << " correlated neutrons, " << multiplicities.size() << " correlated multiplicities, " << muons.size() << " correlated muons\n";

        return true;   
    }

    virtual timestamp ts() const override { return prompt.ts; }

    int run_id;

    vertex prompt;
    vertex delayed;

    std::vector<vertex> neutrons;
    std::vector<vertex> multiplicities;
    std::vector<track> muons;

protected:

    std::shared_ptr<single_event_navigator> m_neu_nav;
    std::shared_ptr<correlator_base> m_neu_corr;
    std::shared_ptr<single_event_navigator> m_mult_nav;
    std::shared_ptr<correlator_base> m_mult_corr;
    std::shared_ptr<muon_event_navigator> m_muon_nav;
    std::shared_ptr<correlator_base> m_muon_corr;

    void append_correlated_vertex(std::shared_ptr<single_event_navigator>& nav, const corrlator_results& res, std::vector<vertex>& v) {
        for (std::ptrdiff_t i = res.lower; i < res.upper; ++i) {
            nav->entry(i);
            v.push_back(nav->single);
        }
    }

    template<typename _Nav>
    void append_correlated_track(std::shared_ptr<_Nav>& nav, const corrlator_results& res) {
        for (std::ptrdiff_t i = res.lower; i < res.upper; ++i) {
            nav->entry(i);
            muons.insert(muons.end(), nav->muons.begin(), nav->muons.end());
        }
    }

};

#endif // JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_