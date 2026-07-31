#include <iostream>

#include <TChain.h>
#include <TFile.h>

#include "analysis/analysis_manager.hpp"
#include "analysis/accidental/accidental_standard_analysis.hpp"
#include "analysis/accidental/accidental_standard_muon_veto_analysis.hpp"
#include "analysis/ibd/ibd_no_neutron_veto_analysis.hpp"
#include "analysis/ibd/ibd_no_neutron_veto_muon_veto_analysis.hpp"
#include "analysis/ibd/ibd_standard_analysis.hpp"
#include "analysis/ibd/ibd_standard_muon_veto_analysis.hpp"
#include "analysis/li9he8/rate/li9he8_rate_muon_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_changing_veto_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_standard_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_with_neutron_analysis.hpp"
#include "analysis/lifetime/daq/daq_total_time_analysis.hpp"
#include "analysis/lifetime/veto/veto_total_time_analysis.hpp"
#include "analysis/multiplicity/multiplicity_standard_analysis.hpp"
#include "analysis/muon/efficiency/muon_wp_tagging_efficiency_analysis.hpp"
#include "analysis/muon/performance/muon_performance_single_analysis.hpp"
#include "analysis/muon/rate/muon_rate_standard_analysis.hpp"
#include "analysis/muon/rate/muon_rate_target_analysis.hpp"
#include "analysis/muon/rate/muon_rate_target_single_analysis.hpp"

timestamp seconds_to_timestamp(double t_sec) {
    time_t sec  = static_cast<time_t>(std::floor(t_sec));
    int nsec = static_cast<int>(std::llround((t_sec - static_cast<double>(sec)) * 1e9));
    if (nsec >= 1000000000L) {
        nsec -= 1000000000L;
        ++sec;
    }
    return timestamp{sec, nsec};
}

std::string format_time_label(double t_sec) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.1f", t_sec);
    std::string s(buf);
    if (s.size() >= 2 && s.substr(s.size() - 2) == ".0") {
        s.erase(s.size() - 2);
    } else {
        for (char& c : s) if (c == '.') c = '_';
    }
    return s + "s";
}

std::string format_dist_label(int d_m) {
    return std::to_string(d_m) + "m";
}

int jrafneck(
    const std::string& analysis_filepath, 
    const std::string& reconstruction_filepath, 
    const std::string& reconstruction_edwin_filepath,
    const std::string& reconstruction_amber_filepath,
    const std::string& reconstruction_tt_filepath,
    const std::string& output_filepath
) {
    std::string suffix = "__OMILREC_JVtx";

    analysis_registry registry;
    analysis_manager manager(output_filepath, registry);

    // --------------------------------------------------------------------------------------------
    // Accidental
    // --------------------------------------------------------------------------------------------

    std::shared_ptr<analysis_base> accidental__standard__analysis__omilrec_jvertex(
        new accidental_standard_analysis(
            "accidental__standard__analysis__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath
        )
    );
    if (!registry.book(accidental__standard__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> accidental__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new accidental_standard_muon_veto_analysis(
            "accidental__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, timestamp{0, 1200000000}, 
            3000.0
        )
    );
    if (!registry.book(accidental__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> accidental__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex(
        new accidental_standard_muon_veto_analysis(
            "accidental__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, timestamp{0, 500000000}, 
            1000.0
        )
    );
    if (!registry.book(accidental__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex)) return 1;

    // --------------------------------------------------------------------------------------------
    // IBD
    // --------------------------------------------------------------------------------------------

    std::shared_ptr<analysis_base> ibd__no_neutron_veto__analysis__omilrec_jvertex(
        new ibd_no_neutron_veto_analysis(
            "ibd__no_neutron_veto__analysis__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath
        )
    );
    if (!registry.book(ibd__no_neutron_veto__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex(
        new ibd_no_neutron_veto_muon_veto_analysis(
            "ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, timestamp{0, 1200000000}, 
            3000.0
        )
    );
    if (!registry.book(ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard__analysis__omilrec_jvertex(
        new ibd_standard_analysis(
            "ibd__standard__analysis__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath
        )
    );
    if (!registry.book(ibd__standard__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new ibd_standard_muon_veto_analysis(
            "ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, timestamp{0, 1200000000}, 
            3000.0
        )
    );
    if (!registry.book(ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex(
        new ibd_standard_muon_veto_analysis(
            "ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, timestamp{0, 500000000}, 
            1000.0
        )
    );
    if (!registry.book(ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex)) return 1;

    // --------------------------------------------------------------------------------------------
    // Li9He8
    // --------------------------------------------------------------------------------------------

    const timestamp sig_start{0, 7000000};
    const timestamp bkg_end{0, -7000000};
    // for (int d_m = 1; d_m <= 10; ++d_m) {
    //     const double dist_mm = static_cast<double>(d_m) * 1000.0;

    //     for (int t_tenths = 2; t_tenths <= 20; t_tenths += 2) {   // 0.2, 0.4, ..., 2.0
    //         const double t_sec = static_cast<double>(t_tenths) / 10.0;

    //         const std::string name =
    //             "li9he8_shape_muon__standard__analysis__cdwpttchi2_" +
    //             format_dist_label(d_m) + "_" + format_time_label(t_sec) +
    //             "__omilrec_jvertex";

    //         const timestamp sig_end   = seconds_to_timestamp(t_sec);
    //         const timestamp bkg_start = seconds_to_timestamp(-t_sec);

    //         std::shared_ptr<analysis_base> analysis(
    //             new li9he8_shape_muon_standard_analysis(
    //                 name,
    //                 analysis_filepath, suffix,
    //                 reconstruction_filepath,
    //                 "CdWpTtChi2",
    //                 sig_start, sig_end,
    //                 bkg_start, bkg_end,
    //                 dist_mm
    //             )
    //         );

    //         if (!registry.book(analysis)) return 1;
    //     }
    // }
    // for (int d_m = 1; d_m <= 10; ++d_m) {
    //     const double dist_mm = static_cast<double>(d_m) * 1000.0;
    //     const double t_sec = 10.0;

    //     const std::string name =
    //         "li9he8_shape_muon__standard__analysis__cdwpttchi2_" +
    //         format_dist_label(d_m) + "_" + format_time_label(t_sec) +
    //         "__omilrec_jvertex";

    //     const timestamp sig_end   = seconds_to_timestamp(t_sec);
    //     const timestamp bkg_start = seconds_to_timestamp(-t_sec);

    //     std::shared_ptr<analysis_base> analysis(
    //         new li9he8_shape_muon_standard_analysis(
    //             name,
    //             analysis_filepath, suffix,
    //             reconstruction_filepath,
    //             "CdWpTtChi2",
    //             sig_start, sig_end,
    //             bkg_start, bkg_end,
    //             dist_mm
    //         )
    //     );

    //     if (!registry.book(analysis)) return 1;
    // }

    std::shared_ptr<analysis_base> li9he8_rate_muon__analysis__omilrec_jvertex(
        new li9he8_rate_muon_analysis(
            "li9he8_rate_muon__analysis__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath
        )
    );
    if (!registry.book(li9he8_rate_muon__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_with_neutron_analysis(
            "li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            sig_start, timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, bkg_end, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_1_5m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_with_neutron_analysis(
            "li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_1_5m_1_2s__omilrec_jvertex", 
            analysis_filepath, suffix, 
            reconstruction_filepath, 
            "CdWpTtChi2", 
            sig_start, timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, bkg_end, 
            1500.0
        )
    );
    if (!registry.book(li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_1_5m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_changing_veto_analysis(
            "li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, suffix,
            reconstruction_filepath, 
            "CdWpTtChi2", 
            sig_start, timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, bkg_end, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_1_55m_1_13s__omilrec_jvertex(
        new li9he8_shape_muon_changing_veto_analysis(
            "li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_1_55m_1_13s__omilrec_jvertex", 
            analysis_filepath, suffix,
            reconstruction_filepath, 
            "CdWpTtChi2", 
            sig_start, timestamp{0, 1130000000}, 
            timestamp{0, -1130000000}, bkg_end, 
            1550.0
        )
    );
    if (!registry.book(li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_1_55m_1_13s__omilrec_jvertex)) return 1;

    // --------------------------------------------------------------------------------------------
    // Multiplicity
    // --------------------------------------------------------------------------------------------

    std::shared_ptr<analysis_base> multiplicity__standard__analysis(
        new multiplicity_standard_analysis(
            "multiplicity__standard__analysis",
            analysis_filepath, "MultiplicityAnalysis__OMILREC_JVtx"
        )
    );
    if (!registry.book(multiplicity__standard__analysis)) return 1;

    // --------------------------------------------------------------------------------------------
    // Lifetime
    // --------------------------------------------------------------------------------------------

    std::shared_ptr<analysis_base> lifetime_daq__total_time__analysis(
        new daq_total_time_analysis(
            "lifetime_daq__total_time__analysis", 
            analysis_filepath, 
            "DAQ"
        )
    );
    if (!registry.book(lifetime_daq__total_time__analysis)) return 1;

    std::shared_ptr<analysis_base> lifetime_veto__total_time__analysis(
        new veto_total_time_analysis(
            "lifetime_veto__total_time__analysis", 
            analysis_filepath, 
            "Veto"
        )
    );
    if (!registry.book(lifetime_veto__total_time__analysis)) return 1;

    // --------------------------------------------------------------------------------------------
    // Muon
    // --------------------------------------------------------------------------------------------

    std::shared_ptr<analysis_base> muon_efficiency__wp_tagging(
        new muon_wp_tagging_efficiency_analysis(
            "muon_efficiency__wp_tagging", 
            reconstruction_filepath, "muons"
        )
    );
    if (!registry.book(muon_efficiency__wp_tagging)) return 1;

    std::shared_ptr<analysis_base> muon_performance__single__analysis(
        new muon_performance_single_analysis(
            "muon_performance__single__analysis", 
            reconstruction_filepath, "muons", 
            reconstruction_edwin_filepath, 
            reconstruction_amber_filepath, 
            reconstruction_tt_filepath, 
            "CdWpTtChi2", "Tt"
        )
    );
    if (!registry.book(muon_performance__single__analysis)) return 1;

    std::shared_ptr<analysis_base> muon_rate__standard__analysis(
        new muon_rate_standard_analysis(
            "muon_rate__standard__analysis", 
            reconstruction_filepath, "muons"
        )
    );
    if (!registry.book(muon_rate__standard__analysis)) return 1;

    std::shared_ptr<analysis_base> muon_rate__target__analysis(
        new muon_rate_target_analysis(
            "muon_rate__target__analysis", 
            reconstruction_filepath, "muons", 
            "CdWpTtChi2"
        )
    );
    if (!registry.book(muon_rate__target__analysis)) return 1;

    std::shared_ptr<analysis_base> muon_rate__target_single__analysis(
        new muon_rate_target_single_analysis(
            "muon_rate__target_single__analysis", 
            reconstruction_filepath, "muons", 
            "CdWpTtChi2"
        )
    );
    if (!registry.book(muon_rate__target_single__analysis)) return 1;


    
    if (!manager.run()) return 1;
    if (!manager.save()) return 1;

    return 0;
}