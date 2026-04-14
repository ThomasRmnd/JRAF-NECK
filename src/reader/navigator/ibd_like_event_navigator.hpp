#ifndef JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTNAVIGATOR_HPP_
#define JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTNAVIGATOR_HPP_

#include "event/track.hpp"
#include "event/vertex.hpp"
#include "reader/containers/vector_reader.hpp"
#include "reader/navigator/navigator.hpp"

class ibd_like_event_navigator : public navigator_base {

public:

    ibd_like_event_navigator(const std::string& filepath, const std::string& treename) :
        navigator_base{filepath, treename}
    {
        if (!is_valid()) return;

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

        m_chain->branch("posx_n", m_posx_n.ptr());
        m_chain->branch("posy_n", m_posy_n.ptr());
        m_chain->branch("posz_n", m_posz_n.ptr());
        m_chain->branch("e_n", m_e_n.ptr());
        m_chain->branch("sec_n", m_sec_n.ptr());
        m_chain->branch("nsec_n", m_nsec_n.ptr());

        m_chain->branch("totq_n", m_totq_n.ptr());
        m_chain->branch("meanq_n", m_meanq_n.ptr());
        m_chain->branch("stdq_n", m_stdq_n.ptr());
        m_chain->branch("minq_n", m_minq_n.ptr());
        m_chain->branch("maxq_n", m_maxq_n.ptr());
        m_chain->branch("meant_n", m_meant_n.ptr());
        m_chain->branch("stdt_n", m_stdt_n.ptr());
        m_chain->branch("npmt_n", m_npmt_n.ptr());
        m_chain->branch("nhit_n", m_nhit_n.ptr());
        m_chain->branch("meanhit_n", m_meanhit_n.ptr());
        m_chain->branch("stdhit_n", m_stdhit_n.ptr());

        m_chain->branch("posx_mult", m_posx_mult.ptr());
        m_chain->branch("posy_mult", m_posy_mult.ptr());
        m_chain->branch("posz_mult", m_posz_mult.ptr());
        m_chain->branch("e_mult", m_e_mult.ptr());
        m_chain->branch("sec_mult", m_sec_mult.ptr());
        m_chain->branch("nsec_mult", m_nsec_mult.ptr());
        m_chain->branch("mult_type", m_mult_type.ptr());

        m_chain->branch("totq_mult", m_totq_mult.ptr());
        m_chain->branch("meanq_mult", m_meanq_mult.ptr());
        m_chain->branch("stdq_mult", m_stdq_mult.ptr());
        m_chain->branch("minq_mult", m_minq_mult.ptr());
        m_chain->branch("maxq_mult", m_maxq_mult.ptr());
        m_chain->branch("meant_mult", m_meant_mult.ptr());
        m_chain->branch("stdt_mult", m_stdt_mult.ptr());
        m_chain->branch("npmt_mult", m_npmt_mult.ptr());
        m_chain->branch("nhit_mult", m_nhit_mult.ptr());
        m_chain->branch("meanhit_mult", m_meanhit_mult.ptr());
        m_chain->branch("stdhit_mult", m_stdhit_mult.ptr());

        m_chain->branch("method_mu", m_method_mu.ptr());
        m_chain->branch("loc_mu", m_loc_mu.ptr());
        m_chain->branch("iposx_mu", m_iposx_mu.ptr());
        m_chain->branch("iposy_mu", m_iposy_mu.ptr());
        m_chain->branch("iposz_mu", m_iposz_mu.ptr());
        m_chain->branch("fposx_mu", m_fposx_mu.ptr());
        m_chain->branch("fposy_mu", m_fposy_mu.ptr());
        m_chain->branch("fposz_mu", m_fposz_mu.ptr());
        m_chain->branch("totq_cd_mu", m_totq_cd_mu.ptr());
        m_chain->branch("totq_wp_mu", m_totq_wp_mu.ptr());
        m_chain->branch("sec_mu", m_sec_mu.ptr());
        m_chain->branch("nsec_mu", m_nsec_mu.ptr());
        m_chain->branch("quality_mu", m_quality_mu.ptr());
    }

    virtual ~ibd_like_event_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!navigator_base::entry(n)) return false;

        neutrons.clear();
        neutrons.reserve(m_posx_n.size());
        for (std::size_t k = 0ul; k < m_posx_n.size(); ++k) {
            neutrons.emplace_back(
                m_e_n[k], vec3{m_posx_n[k], m_posy_n[k], m_posz_n[k]}, timestamp{m_sec_n[k], m_nsec_n[k]},
                m_totq_n[k], m_meanq_n[k], m_stdq_n[k], m_minq_n[k], m_maxq_n[k], m_meant_n[k], m_stdt_n[k], m_npmt_n[k], m_nhit_n[k], m_meanhit_n[k], m_stdhit_n[k]
            );
        }

        multiplicities.clear();
        multiplicities.reserve(m_posx_mult.size());
        for (std::size_t k = 0ul; k < m_posx_mult.size(); ++k) {
            multiplicities.emplace_back(
                m_e_mult[k], vec3{m_posx_mult[k], m_posy_mult[k], m_posz_mult[k]}, timestamp{m_sec_mult[k], m_nsec_mult[k]},
                m_totq_mult[k], m_meanq_mult[k], m_stdq_mult[k], m_minq_mult[k], m_maxq_mult[k], m_meant_mult[k], m_stdt_mult[k], m_npmt_mult[k], m_nhit_mult[k], m_meanhit_mult[k], m_stdhit_mult[k]
            );
        }

        muons.clear();
        muons.reserve(m_method_mu.size());
        for (std::size_t k = 0ul; k < m_method_mu.size(); ++k) {
            muons.emplace_back(
                m_method_mu[k], m_totq_cd_mu[k], m_totq_wp_mu[k], 
                vec3{m_iposx_mu[k], m_iposy_mu[k], m_iposz_mu[k]},
                vec3{m_fposx_mu[k], m_fposy_mu[k], m_fposz_mu[k]},
                timestamp{m_sec_mu[k], m_nsec_mu[k]},
                m_quality_mu[k], m_loc_mu[k]
            );
        }

        return true;
    }

    int run_id;

    vertex prompt;
    vertex delayed;

    std::vector<vertex> neutrons;
    std::vector<vertex> multiplicities;
    std::vector<track> muons;

protected:

    vector_reader<double> m_posx_n;
    vector_reader<double> m_posy_n;
    vector_reader<double> m_posz_n;
    vector_reader<double> m_e_n;
    vector_reader<time_t> m_sec_n;
    vector_reader<int> m_nsec_n;

    vector_reader<double> m_totq_n;
    vector_reader<double> m_meanq_n;
    vector_reader<double> m_stdq_n;
    vector_reader<double> m_minq_n;
    vector_reader<double> m_maxq_n;
    vector_reader<double> m_meant_n;
    vector_reader<double> m_stdt_n;
    vector_reader<std::size_t> m_npmt_n;
    vector_reader<std::size_t> m_nhit_n;
    vector_reader<double> m_meanhit_n;
    vector_reader<double> m_stdhit_n;

    vector_reader<double> m_posx_mult;
    vector_reader<double> m_posy_mult;
    vector_reader<double> m_posz_mult;
    vector_reader<double> m_e_mult;
    vector_reader<time_t> m_sec_mult;
    vector_reader<int> m_nsec_mult;
    vector_reader<int> m_mult_type;

    vector_reader<double> m_totq_mult;
    vector_reader<double> m_meanq_mult;
    vector_reader<double> m_stdq_mult;
    vector_reader<double> m_minq_mult;
    vector_reader<double> m_maxq_mult;
    vector_reader<double> m_meant_mult;
    vector_reader<double> m_stdt_mult;
    vector_reader<std::size_t> m_npmt_mult;
    vector_reader<std::size_t> m_nhit_mult;
    vector_reader<double> m_meanhit_mult;
    vector_reader<double> m_stdhit_mult;

    vector_reader<std::string> m_method_mu;
    vector_reader<int> m_loc_mu;
    vector_reader<double> m_iposx_mu;
    vector_reader<double> m_iposy_mu;
    vector_reader<double> m_iposz_mu;
    vector_reader<double> m_fposx_mu;
    vector_reader<double> m_fposy_mu;
    vector_reader<double> m_fposz_mu;
    vector_reader<double> m_totq_cd_mu;
    vector_reader<double> m_totq_wp_mu;
    vector_reader<time_t> m_sec_mu;
    vector_reader<int> m_nsec_mu;
    vector_reader<double> m_quality_mu;

};

#endif // JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTNAVIGATOR_HPP_