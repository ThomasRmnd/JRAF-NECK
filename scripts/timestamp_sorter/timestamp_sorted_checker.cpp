#include <iostream>

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

int timestamp_sorted_checker(const char* input, const char* treename) {

    TFile* fin = TFile::Open(input, "READ");
    if (!fin) {
        std::cerr << "Error: cannot open file " << input << '\n';
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

    const Long64_t nentries = tin->GetEntries();
    if (nentries == 0) {
        std::cout << "Tree " << treename << " is empty (already sorted)\n";
        return 0;
    }

    tin->GetEntry(0);
    timestamp_value prev = reader->get();

    for (Long64_t k = 1l; k < nentries; ++k) {
        tin->GetEntry(k);
        timestamp_value cur = reader->get();

        if (cur < prev) {
            std::cerr << "Error: file " << input << " is NOT sorted at entry " << k << "\n";
            std::cerr << "   previous: (" << prev.sec << ", " << prev.nsec << ")\n";
            std::cerr << "   current : (" << cur.sec  << ", " << cur.nsec  << ")\n";
            return 1;
        }

        prev = cur;
    }

    std::cout << "Info: tree is timestamp sorted (" << nentries << " entries)\n";
    return 0;
}