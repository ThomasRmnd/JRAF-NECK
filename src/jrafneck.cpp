#include <iostream>

#include <TChain.h>
#include <TFile.h>

#include "analysis/analysis_manager.hpp"
#include "analysis/ibd/ibd_no_neutron_veto_analysis.hpp"
#include "analysis/ibd/ibd_no_neutron_veto_muon_veto_analysis.hpp"
#include "analysis/ibd/ibd_standard_analysis.hpp"
#include "analysis/ibd/ibd_standard_muon_veto_analysis.hpp"
// #include "analysis/cosmogenic/cosmo_rate_analysis.hpp"
// #include "analysis/cosmogenic/cosmo_rate_neutron_veto_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_changing_veto_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_standard_analysis.hpp"
#include "analysis/li9he8/shape/li9he8_shape_muon_with_neutron_analysis.hpp"
#include "analysis/lifetime/daq/daq_total_time_analysis.hpp"
#include "analysis/lifetime/veto/veto_total_time_analysis.hpp"
#include "analysis/muon/performance/muon_performance_single_analysis.hpp"
#include "analysis/muon/rate/muon_rate_analysis.hpp"

int jrafneck(const std::string& analysis_filepath, const std::string& reconstruction_filepath, const std::string& output_filepath) {
    std::string suffix = "__OMILREC_JVtx";

    analysis_registry registry;
    analysis_manager manager(output_filepath, registry);



    std::shared_ptr<analysis_base> ibd__no_neutron_veto__analysis__omilrec_jvertex(
        new ibd_no_neutron_veto_analysis(
            "ibd__no_neutron_veto__analysis__omilrec_jvertex", 
            analysis_filepath, 
            suffix
        )
    );
    if (!registry.book(ibd__no_neutron_veto__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex(
        new ibd_no_neutron_veto_muon_veto_analysis(
            "ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 1200000000}, 
            3000.0
        )
    );
    if (!registry.book(ibd__no_neutron_veto_muon_veto__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard__analysis__omilrec_jvertex(
        new ibd_standard_analysis(
            "ibd__standard__analysis__omilrec_jvertex", 
            analysis_filepath, 
            suffix
        )
    );
    if (!registry.book(ibd__standard__analysis__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new ibd_standard_muon_veto_analysis(
            "ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 1200000000}, 
            3000.0
        )
    );
    if (!registry.book(ibd__standard_muon_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex(
        new ibd_standard_muon_veto_analysis(
            "ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 500000000}, 
            1000.0
        )
    );
    if (!registry.book(ibd__standard_muon_veto__analysis__cdwpttchi2_1m_0_5s__omilrec_jvertex)) return 1;



    // std::shared_ptr<analysis_base> cosmo_rate_analysis_omilrec_jvertex(
    //     new cosmo_rate_analysis(
    //         "cosmo_rate_analysis_omilrec_jvertex", 
    //         analysis_filepath, 
    //         suffix
    //     )
    // );
    // if (!registry.book(cosmo_rate_analysis_omilrec_jvertex)) return 1;

    // std::shared_ptr<analysis_base> cosmo_rate_neutron_veto_analysis_omilrec_jvertex(
    //     new cosmo_rate_neutron_veto_analysis(
    //         "cosmo_rate_neutron_veto_analysis_omilrec_jvertex", 
    //         analysis_filepath, 
    //         suffix
    //     )
    // );
    // if (!registry.book(cosmo_rate_neutron_veto_analysis_omilrec_jvertex)) return 1;



    std::shared_ptr<analysis_base> li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_standard_analysis(
            "li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, 
            timestamp{0, -5000000}, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_2s__omilrec_jvertex(
        new li9he8_shape_muon_standard_analysis(
            "li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_2s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{2, 0}, 
            timestamp{-2, 0}, 
            timestamp{0, -5000000}, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__standard__analysis__cdwpttchi2_3m_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_with_neutron_analysis(
            "li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, 
            timestamp{0, -5000000}, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__with_neutron__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;

    std::shared_ptr<analysis_base> li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex(
        new li9he8_shape_muon_changing_veto_analysis(
            "li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex", 
            analysis_filepath, 
            suffix, 
            "CdWpTtChi2", 
            timestamp{0, 5000000}, 
            timestamp{0, 1200000000}, 
            timestamp{0, -1200000000}, 
            timestamp{0, -5000000}, 
            3000.0
        )
    );
    if (!registry.book(li9he8_shape_muon__changing_veto__analysis__cdwpttchi2_3m_1_2s__omilrec_jvertex)) return 1;



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



    std::shared_ptr<analysis_base> muon_performance__single__analysis(
        new muon_performance_single_analysis(
            "muon_performance__single__analysis", 
            reconstruction_filepath, 
            "muons", 
            "CdWpTtChi2", 
            "Tt"
        )
    );
    if (!registry.book(muon_performance__single__analysis)) return 1;

    std::shared_ptr<analysis_base> muon_rate__analysis(
        new muon_rate_analysis(
            "muon_rate__analysis", 
            reconstruction_filepath, 
            "muons"
        )
    );
    // if (!registry.book(muon_rate__analysis)) return 1;


    
    if (!manager.run()) return 1;
    if (!manager.save()) return 1;

    return 0;
}