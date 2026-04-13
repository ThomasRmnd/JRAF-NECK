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

        m_chain->branch("posx_n", posx_n.ptr());
        m_chain->branch("posy_n", posy_n.ptr());
        m_chain->branch("posz_n", posz_n.ptr());
        m_chain->branch("e_n", e_n.ptr());
        m_chain->branch("sec_n", sec_n.ptr());
        m_chain->branch("nsec_n", nsec_n.ptr());

        m_chain->branch("totq_n", totq_n.ptr());
        m_chain->branch("meanq_n", meanq_n.ptr());
        m_chain->branch("stdq_n", stdq_n.ptr());
        m_chain->branch("minq_n", minq_n.ptr());
        m_chain->branch("maxq_n", maxq_n.ptr());
        m_chain->branch("meant_n", meant_n.ptr());
        m_chain->branch("stdt_n", stdt_n.ptr());
        m_chain->branch("npmt_n", npmt_n.ptr());
        m_chain->branch("nhit_n", nhit_n.ptr());
        m_chain->branch("meanhit_n", meanhit_n.ptr());
        m_chain->branch("stdhit_n", stdhit_n.ptr());

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
        m_chain->branch("iposx_mu", iposx_mu.ptr());
        m_chain->branch("iposy_mu", iposy_mu.ptr());
        m_chain->branch("iposz_mu", iposz_mu.ptr());
        m_chain->branch("fposx_mu", fposx_mu.ptr());
        m_chain->branch("fposy_mu", fposy_mu.ptr());
        m_chain->branch("fposz_mu", fposz_mu.ptr());
        m_chain->branch("totq_cd_mu", totq_cd_mu.ptr());
        m_chain->branch("totq_wp_mu", totq_wp_mu.ptr());
        m_chain->branch("sec_mu", sec_mu.ptr());
        m_chain->branch("nsec_mu", nsec_mu.ptr());
        m_chain->branch("quality_mu", quality_mu.ptr());
    }

    virtual ~ibd_like_event_navigator() override = default;

    virtual bool entry(std::ptrdiff_t n) override {
        if (!m_chain->entry(n)) return false;

        neutrons.clear();
        neutrons.reserve(posx_n.size());
        for (std::size_t k = 0ul; k < posx_n.size(); ++k) {
            neutrons.emplace_back(
                e_n[k], vec3{posx_n[k], posy_n[k], posz_n[k]}, timestamp{sec_n[k], nsec_n[k]},
                totq_n[k], meanq_n[k], stdq_n[k], minq_n[k], maxq_n[k], meant_n[k], stdt_n[k], npmt_n[k], nhit_n[k], meanhit_n[k], stdhit_n[k]
            );
        }

        multiplicities.clear();
        multiplicities.reserve(posx_mult.size());
        for (std::size_t k = 0ul; k < posx_mult.size(); ++k) {
            multiplicities.emplace_back(
                e_mult[k], vec3{posx_mult[k], posy_mult[k], posz_mult[k]}, timestamp{sec_mult[k], nsec_mult[k]},
                totq_mult[k], meanq_mult[k], stdq_mult[k], minq_mult[k], maxq_mult[k], meant_mult[k], stdt_mult[k], npmt_mult[k], nhit_mult[k], meanhit_mult[k], stdhit_mult[k]
            );
        }

        muons.clear();
        muons.reserve(method_mu.size());
        for (std::size_t k = 0ul; k < method_mu.size(); ++k) {
            muons.emplace_back(
                method_mu[k], totq_cd_mu[k], totq_wp_mu[k], 
                vec3{iposx_mu[k], iposy_mu[k], iposz_mu[k]},
                vec3{fposx_mu[k], fposy_mu[k], fposz_mu[k]},
                timestamp{sec_mu[k], nsec_mu[k]},
                quality_mu[k], loc_mu[k]
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

    vector_reader<double> posx_n;
    vector_reader<double> posy_n;
    vector_reader<double> posz_n;
    vector_reader<double> e_n;
    vector_reader<time_t> sec_n;
    vector_reader<int> nsec_n;

    vector_reader<double> totq_n;
    vector_reader<double> meanq_n;
    vector_reader<double> stdq_n;
    vector_reader<double> minq_n;
    vector_reader<double> maxq_n;
    vector_reader<double> meant_n;
    vector_reader<double> stdt_n;
    vector_reader<std::size_t> npmt_n;
    vector_reader<std::size_t> nhit_n;
    vector_reader<double> meanhit_n;
    vector_reader<double> stdhit_n;

    vector_reader<double> posx_mult;
    vector_reader<double> posy_mult;
    vector_reader<double> posz_mult;
    vector_reader<double> e_mult;
    vector_reader<time_t> sec_mult;
    vector_reader<int> nsec_mult;
    vector_reader<int> mult_type;

    vector_reader<double> totq_mult;
    vector_reader<double> meanq_mult;
    vector_reader<double> stdq_mult;
    vector_reader<double> minq_mult;
    vector_reader<double> maxq_mult;
    vector_reader<double> meant_mult;
    vector_reader<double> stdt_mult;
    vector_reader<std::size_t> npmt_mult;
    vector_reader<std::size_t> nhit_mult;
    vector_reader<double> meanhit_mult;
    vector_reader<double> stdhit_mult;

    vector_reader<std::string> method_mu;
    vector_reader<int> loc_mu;
    vector_reader<double> iposx_mu;
    vector_reader<double> iposy_mu;
    vector_reader<double> iposz_mu;
    vector_reader<double> fposx_mu;
    vector_reader<double> fposy_mu;
    vector_reader<double> fposz_mu;
    vector_reader<double> totq_cd_mu;
    vector_reader<double> totq_wp_mu;
    vector_reader<time_t> sec_mu;
    vector_reader<int> nsec_mu;
    vector_reader<double> quality_mu;

};

#endif // JRAFNECK_READER_NAVIGATOR_IBDLIKEEVENTNAVIGATOR_HPP_