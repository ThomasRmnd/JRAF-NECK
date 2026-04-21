#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <TFile.h>
#include <TTree.h>

struct timestamp_value {
    time_t sec;
    int nsec;
};

bool operator<(const timestamp_value& lhs, const timestamp_value& rhs) {
    return lhs.sec < rhs.sec || (lhs.sec == rhs.sec && lhs.nsec < rhs.nsec);
}

class timestamp_reader {

public:

    virtual ~timestamp_reader() = default;

    virtual void setup(TTree* tree) = 0;
    virtual timestamp_value get() const = 0;

};

class amber_timestamp_reader : public timestamp_reader {

public:

    ~amber_timestamp_reader() override = default;

    void setup(TTree* tree) override {
        tree->SetBranchAddress("fSec", &m_fSec);
        tree->SetBranchAddress("fNanoSec", &m_fNanoSec);
    }

    timestamp_value get() const override {
        return {static_cast<time_t>(m_fSec), static_cast<int>(m_fNanoSec)};
    }

private:

    int m_fSec;
    int m_fNanoSec;

};

class edwin_timestamp_reader : public timestamp_reader {

public:

    ~edwin_timestamp_reader() override = default;

    void setup(TTree* tree) override {
        tree->SetBranchAddress("cd_time_s", &sec);
        tree->SetBranchAddress("cd_time_ns", &nsec);
    }

    timestamp_value get() const override {
        return {static_cast<time_t>(sec), static_cast<int>(nsec)};
    }

private:

    long long sec;
    long long nsec;

};

std::unique_ptr<timestamp_reader> make_reader(const std::string& treename) {
    if (treename == "MuonReco") {
        return std::make_unique<amber_timestamp_reader>();
    }
    if (treename == "Edwin_Muon") {
        return std::make_unique<edwin_timestamp_reader>();
    }
    return nullptr;
}

struct entry {
    
    Long64_t index;
    timestamp_value ts;

};

bool operator<(const entry& lhs, const entry& rhs) {
    return lhs.ts < rhs.ts;
}

int timestamp_sorter(const char* input, const char* treename, const char* output) {
    TFile* fin = TFile::Open(input, "READ");
    if (!fin) {
        std::cerr << "Error: cannot open input file " << input << '\n';
        return 1;
    }

    TTree* tin = fin->Get<TTree>(treename);
    if (!tin) {
        std::cerr << "Error: cannot retrieve tree " << treename << " in file " << input << '\n';
        return 1;
    }

    std::unique_ptr<timestamp_reader> reader = make_reader(treename);
    if (!reader) {
        std::cerr << "Error: cannot create timestamp reader with tree " << treename << '\n';
        return 1;
    }
    reader->setup(tin);

    Long64_t nentries = tin->GetEntries();

    std::vector<entry> ordering;
    ordering.reserve(nentries);

    for (Long64_t i = 0; i < nentries; ++i) {
        tin->GetEntry(i);
        ordering.push_back({i, reader->get()});
    }

    std::sort(ordering.begin(), ordering.end());

    TFile* fout = TFile::Open(output, "RECREATE");
    if (!fout) {
        std::cerr << "Error: cannot open output file " << output << '\n';
        return 1;
    }

    TTree* tout = tin->CloneTree(0);
    if (!tout) {
        std::cerr << "Error: cannot clone tree " << treename << '\n';
        return 1;
    }

    for (const entry& e : ordering) {
        tin->GetEntry(e.index);
        tout->Fill();
    }

    fout->cd();
    tout->Write();
    fout->Close();
    fin->Close();

    std::cout << "Info: sorted tree written to " << output << '\n';

    return 0;
}