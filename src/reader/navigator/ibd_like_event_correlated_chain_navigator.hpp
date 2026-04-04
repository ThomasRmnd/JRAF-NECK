#ifndef JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_

#include "event/vertex.hpp"
#include "reader/containers/vector_reader.hpp"
#include "reader/navigator/navigator.hpp"
#include "utils/timestamp.hpp"

namespace jraf {

struct neutron {

    int run_id;
    vertex neu;
    vertex_metadata meta_neu;

};

class ibd_like_event_correlated_chain_navigator : public jraf::navigator_base {

public:

    ibd_like_event_correlated_chain_navigator(const std::string& filepath, const std::string& treename, const std::string& neutron_treename, const jraf::timestamp& low_bound, const jraf::timestamp& high_bound) :
        jraf::navigator_base{filepath, treename},
        m_low_bound{low_bound},
        m_high_bound{high_bound}
    {
        if (!is_valid()) return;

        m_chain_neutron = jraf::chain_reader_manager::retrieve(filepath, neutron_treename);
        if (!m_chain_neutron->is_valid()) {
            std::cerr << "Neutron chain of filepath " << filepath << " and treename " << neutron_treename << " is not valid\n";
            return;
        }

        m_chain->branch("run_id", run_id);

        m_chain->branch("posx_p", prompt.pos.x);
        m_chain->branch("posy_p", prompt.pos.y);
        m_chain->branch("posz_p", prompt.pos.z);
        m_chain->branch("e_p", prompt.e);
        m_chain->branch("sec_p", prompt.ts.sec);
        m_chain->branch("nsec_p", prompt.ts.nsec);

        m_chain->branch("totq_p", meta_prompt.totq);
        m_chain->branch("meanq_p", meta_prompt.meanq);
        m_chain->branch("stdq_p", meta_prompt.stdq);
        m_chain->branch("minq_p", meta_prompt.minq);
        m_chain->branch("maxq_p", meta_prompt.maxq);
        m_chain->branch("meant_p", meta_prompt.meant);
        m_chain->branch("stdt_p", meta_prompt.stdt);
        m_chain->branch("npmt_p", meta_prompt.npmt);
        m_chain->branch("nhit_p", meta_prompt.nhit);
        m_chain->branch("meanhit_p", meta_prompt.meanhit);
        m_chain->branch("stdhit_p", meta_prompt.stdhit);

        m_chain->branch("posx_d", delayed.pos.x);
        m_chain->branch("posy_d", delayed.pos.y);
        m_chain->branch("posz_d", delayed.pos.z);
        m_chain->branch("e_d", delayed.e);
        m_chain->branch("sec_d", delayed.ts.sec);
        m_chain->branch("nsec_d", delayed.ts.nsec);

        m_chain->branch("totq_d", meta_delayed.totq);
        m_chain->branch("meanq_d", meta_delayed.meanq);
        m_chain->branch("stdq_d", meta_delayed.stdq);
        m_chain->branch("minq_d", meta_delayed.minq);
        m_chain->branch("maxq_d", meta_delayed.maxq);
        m_chain->branch("meant_d", meta_delayed.meant);
        m_chain->branch("stdt_d", meta_delayed.stdt);
        m_chain->branch("npmt_d", meta_delayed.npmt);
        m_chain->branch("nhit_d", meta_delayed.nhit);
        m_chain->branch("meanhit_d", meta_delayed.meanhit);
        m_chain->branch("stdhit_d", meta_delayed.stdhit);

        m_chain_neutron->branch("run_id", m_neutron.run_id);
        m_chain_neutron->branch("posx", m_neutron.neu.pos.x);
        m_chain_neutron->branch("posy", m_neutron.neu.pos.y);
        m_chain_neutron->branch("posz", m_neutron.neu.pos.z);
        m_chain_neutron->branch("e", m_neutron.neu.e);
        m_chain_neutron->branch("sec", m_neutron.neu.ts.sec);
        m_chain_neutron->branch("nsec", m_neutron.neu.ts.nsec);

        m_chain_neutron->branch("totq", m_neutron.meta_neu.totq);
        m_chain_neutron->branch("meanq", m_neutron.meta_neu.meanq);
        m_chain_neutron->branch("stdq", m_neutron.meta_neu.stdq);
        m_chain_neutron->branch("minq", m_neutron.meta_neu.minq);
        m_chain_neutron->branch("maxq", m_neutron.meta_neu.maxq);
        m_chain_neutron->branch("meant", m_neutron.meta_neu.meant);
        m_chain_neutron->branch("stdt", m_neutron.meta_neu.stdt);
        m_chain_neutron->branch("npmt", m_neutron.meta_neu.npmt);
        m_chain_neutron->branch("nhit", m_neutron.meta_neu.nhit);
        m_chain_neutron->branch("meanhit", m_neutron.meta_neu.meanhit);
        m_chain_neutron->branch("stdhit", m_neutron.meta_neu.stdhit);

        m_chain->branch("posx_mult", posx_mult.ptr());
        m_chain->branch("posy_mult", posy_mult.ptr());
        m_chain->branch("posz_mult", posz_mult.ptr());
        m_chain->branch("e_mult", e_mult.ptr());
        m_chain->branch("sec_mult", sec_mult.ptr());
        m_chain->branch("nsec_mult", nsec_mult.ptr());
        m_chain->branch("mult_type", mult_type.ptr());

        m_chain->branch("totq_mult", totq_mult.ptr());
        m_chain->branch("meanq_mult", meanq_mult.ptr());
        m_chain->branch("stdq_mult", stdq_mult.ptr());
        m_chain->branch("minq_mult", minq_mult.ptr());
        m_chain->branch("maxq_mult", maxq_mult.ptr());
        m_chain->branch("meant_mult", meant_mult.ptr());
        m_chain->branch("stdt_mult", stdt_mult.ptr());
        m_chain->branch("npmt_mult", npmt_mult.ptr());
        m_chain->branch("nhit_mult", nhit_mult.ptr());
        m_chain->branch("meanhit_mult", meanhit_mult.ptr());
        m_chain->branch("stdhit_mult", stdhit_mult.ptr());

        m_chain->branch("method_mu", method_mu.ptr());
        m_chain->branch("loc_mu", loc_mu.ptr());
        m_chain->branch("posx_mu", posx_mu.ptr());
        m_chain->branch("posy_mu", posy_mu.ptr());
        m_chain->branch("posz_mu", posz_mu.ptr());
        m_chain->branch("dirx_mu", dirx_mu.ptr());
        m_chain->branch("diry_mu", diry_mu.ptr());
        m_chain->branch("dirz_mu", dirz_mu.ptr());
        m_chain->branch("totq_cd_mu", totq_cd_mu.ptr());
        m_chain->branch("totq_wp_mu", totq_wp_mu.ptr());
        m_chain->branch("sec_mu", sec_mu.ptr());
        m_chain->branch("nsec_mu", nsec_mu.ptr());
        m_chain->branch("quality_mu", quality_mu.ptr());
    }

    virtual ~ibd_like_event_correlated_chain_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) {
        if (!jraf::navigator_base::entry(n)) return false;
        
        jraf::timestamp ts = prompt.ts;
        jraf::timestamp low_ts = prompt.ts + m_low_bound;
        jraf::timestamp high_ts = prompt.ts + m_high_bound;

        // binary search in the neutron chain
        std::size_t left = 0ul;
        std::size_t right = m_chain_neutron->size();
        while (left < right) {
            std::size_t mid = left + (right - left) / 2;
            m_chain_neutron->entry(mid);
            timestamp mid_ts = m_neutron.neu.ts;
            if (mid_ts < low_ts) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        std::size_t low_idx = left;

        left = 0ul;
        right = m_chain_neutron->size();
        while (left < right) {
            std::size_t mid = left + (right - left) / 2;
            m_chain_neutron->entry(mid);
            timestamp mid_ts = m_neutron.neu.ts;
            if (mid_ts <= high_ts) {
                left = mid + 1;
            }
            else {
                right = mid;
            }
        }
        std::size_t high_idx = left;

        for (std::size_t i = low_idx; i < high_idx; ++i) {
            m_chain_neutron->entry(i);
            neutrons.push_back(m_neutron);
        }

        return true;
    }

    int run_id;

    jraf::vertex prompt;
    jraf::vertex delayed;

    jraf::vertex_metadata meta_prompt;
    jraf::vertex_metadata meta_delayed;

    std::vector<neutron> neutrons;

    jraf::vector_reader<double> posx_mult;
    jraf::vector_reader<double> posy_mult;
    jraf::vector_reader<double> posz_mult;
    jraf::vector_reader<double> e_mult;
    jraf::vector_reader<time_t> sec_mult;
    jraf::vector_reader<int> nsec_mult;
    jraf::vector_reader<int> mult_type;

    jraf::vector_reader<double> totq_mult;
    jraf::vector_reader<double> meanq_mult;
    jraf::vector_reader<double> stdq_mult;
    jraf::vector_reader<double> minq_mult;
    jraf::vector_reader<double> maxq_mult;
    jraf::vector_reader<double> meant_mult;
    jraf::vector_reader<double> stdt_mult;
    jraf::vector_reader<std::size_t> npmt_mult;
    jraf::vector_reader<std::size_t> nhit_mult;
    jraf::vector_reader<double> meanhit_mult;
    jraf::vector_reader<double> stdhit_mult;

    jraf::vector_reader<std::string> method_mu;
    jraf::vector_reader<int> loc_mu;
    jraf::vector_reader<double> posx_mu;
    jraf::vector_reader<double> posy_mu;
    jraf::vector_reader<double> posz_mu;
    jraf::vector_reader<double> dirx_mu;
    jraf::vector_reader<double> diry_mu;
    jraf::vector_reader<double> dirz_mu;
    jraf::vector_reader<double> totq_cd_mu;
    jraf::vector_reader<double> totq_wp_mu;
    jraf::vector_reader<time_t> sec_mu;
    jraf::vector_reader<int> nsec_mu;
    jraf::vector_reader<double> quality_mu;

private:

    std::shared_ptr<jraf::chain_reader> m_chain_neutron;
    neutron m_neutron;
    jraf::timestamp m_low_bound;
    jraf::timestamp m_high_bound;

};

} // namespace jraf

#endif // JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTCORRELATEDCHAINNAVIGATOR_HPP_