#include <iostream>
#include <unordered_set>
#include <string>

#include <TChain.h>
#include <TFile.h>
#include <TTimeStamp.h>
#include <TTree.h>
#include <TVector3.h>

struct TtUserResoChain {

    bool open(const char* filepath) {
        if (chain) {
            delete chain;
        }
        chain = new TChain("TT");
        if (!chain) {
            std::cerr << "Cannot create chain TT\n";
            return false;
        }
        chain->Add(filepath);
        chain->SetBranchAddress("evtID", &evtID);
        chain->SetBranchAddress("NTotPoints", &NTotPoints);
        chain->SetBranchAddress("PointX", &PointX);
        chain->SetBranchAddress("PointY", &PointY);
        chain->SetBranchAddress("PointZ", &PointZ);
        chain->SetBranchAddress("NTracks", &NTracks);
        chain->SetBranchAddress("NPoints", &NPoints);
        chain->SetBranchAddress("Coeff0", &Coeff0);
        chain->SetBranchAddress("Coeff1", &Coeff1);
        chain->SetBranchAddress("Coeff2", &Coeff2);
        chain->SetBranchAddress("Coeff3", &Coeff3);
        chain->SetBranchAddress("Coeff4", &Coeff4);
        chain->SetBranchAddress("Coeff5", &Coeff5);
        chain->SetBranchAddress("Chi2", &Chi2);
        chain->SetBranchAddress("start_TS", &start_TS);
        return true;
    }

    std::size_t size() const {
        return static_cast<std::size_t>(chain->GetEntries());
    }

    int entry(std::ptrdiff_t n) {
        return chain->GetEntry(n);
    }

    TChain* chain = nullptr;

    int evtID;
    int NTotPoints;
    float PointX[20];
    float PointY[20];
    float PointZ[20];
    int NTracks;
    int NPoints[20];
    double Coeff0[20];
    double Coeff1[20];
    double Coeff2[20];
    double Coeff3[20];
    double Coeff4[20];
    double Coeff5[20];
    double Chi2[20];
    TTimeStamp* start_TS = nullptr;

};

int get_tt_layer_id(double z) {
    if (24000.0 <= z && z <= 25000.0) return 0;  // main
    if (25500.0 <= z && z <= 26500.0) return 1;  // main
    if (27000.0 <= z && z <= 28000.0) return 2;  // main
    if (30000.0 <= z && z <= 30200.0) return 3;  // chimney
    if (30200.0 <= z && z <= 30400.0) return 4;  // chimney
    if (30400.0 <= z && z <= 30600.0) return 5;  // chimney
    return -1; // not inside any valid layer
}

int tt_reco_extractor(const char* input, int run, const char* output) {
    TtUserResoChain tt_user_reco_chain;
    if (!tt_user_reco_chain.open(input)) return 1;

    TFile* file = TFile::Open(output, "RECREATE");
    if (!file) {
        std::cerr << "Cannot open file " << output << '\n';
        return 1;
    }
    TTree* tree = new TTree("single_muon", "single_muon");
    if (!tree) {
        std::cerr << "Cannot create tree single_muon\n";
        return 1;
    }

    int runid;
    time_t sec;
    int nsec;
    double iposx, iposy, iposz;
    double fposx, fposy, fposz;
    double chi2;
    tree->Branch("runid", &runid);
    tree->Branch("sec", &sec);
    tree->Branch("nsec", &nsec);
    tree->Branch("iposx", &iposx);
    tree->Branch("iposy", &iposy);
    tree->Branch("iposz", &iposz);
    tree->Branch("fposx", &fposx);
    tree->Branch("fposy", &fposy);
    tree->Branch("fposz", &fposz);
    tree->Branch("chi2", &chi2);

    std::unordered_set<int> layers_hit;
    TVector3 ipos, dir, fpos;

    std::ptrdiff_t size = static_cast<std::ptrdiff_t>(tt_user_reco_chain.size());
    for (std::ptrdiff_t k = 0ll; k < size; ++k) {
        tt_user_reco_chain.entry(k);
        if (tt_user_reco_chain.NTracks != 1) continue;
        if (tt_user_reco_chain.NPoints[0] < 3) continue;

        layers_hit.clear();
        layers_hit.reserve(6);
        for (int i = 0; i < tt_user_reco_chain.NTotPoints; ++i) {
            int lid = get_tt_layer_id(tt_user_reco_chain.PointZ[i] + 26452.0);
            if (lid < 0) continue;
            layers_hit.insert(lid);
        }
        if (layers_hit.size() < 3) continue;

        ipos.SetXYZ(tt_user_reco_chain.Coeff0[0], tt_user_reco_chain.Coeff1[0], tt_user_reco_chain.Coeff2[0] + 26452.0);
        dir = TVector3(tt_user_reco_chain.Coeff3[0], tt_user_reco_chain.Coeff4[0], tt_user_reco_chain.Coeff5[0]).Unit();
        fpos = ipos - 2.0 * (ipos * dir) * dir;

        runid = run;
        sec = tt_user_reco_chain.start_TS->GetSec();
        nsec = tt_user_reco_chain.start_TS->GetNanoSec();
        iposx = ipos.X();
        iposy = ipos.Y();
        iposz = ipos.Z();
        fposx = fpos.X();
        fposy = fpos.Y();
        fposz = fpos.Z();
        chi2 = tt_user_reco_chain.Chi2[0];
        tree->Fill();
    }

    file->cd();
    tree->Write();
    file->Write();
    file->Close();

    return 0;
}