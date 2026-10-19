#include <ROOT/RDF/HistoModels.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RResultPtr.hxx>
#include <RtypesCore.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TString.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

std::map<std::string, std::string> MC_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/MC_2025_d25e5b4/"},
};
std::map<std::string, std::string> DATA_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/Data_2025_d25e5b4/"},
};

// clang-format off
const std::string MC_dataSets[] = {
    "DYJetsTo2E", "DYJetsTo2Mu", "ggH124", "ggH124p5", "ggH125",
    "ggH125p5", "ggH126", "ggTo2e2mu_Contin_MCFM701", "ggTo2e2tau_Contin_MCFM701",
    "ggTo2mu2tau_Contin_MCFM701", "ggTo4e_Contin_MCFM701", "ggTo4mu_Contin_MCFM701",
    "ggTo4tau_Contin_MCFM701", "ttH124", "ttH124p5", "ttH125",
    "ttH125p5", "ttH126", "TTto2L2Nu", "TTWW", "TTZZ", "VBFH124",
    "VBFH124p5", "VBFH125", "VBFH125p5", "VBFH126", "WminusH124",
    "WminusH124p5", "WminusH125", "WminusH125p5", "WminusH126",
    "WplusH124", "WplusH124p5", "WplusH125", "WplusH125p5",
    "WplusH126", "WWZ", "WZto3LNu", "WZZ", "ZH124", "ZH124p5",
    "ZH125", "ZH125p5", "ZH126", "ZZTo4l", "ZZZ",
};
// clang-format on

std::vector<TFile *> openFiles(std::vector<std::string> fileNames) {
  std::vector<TFile *> files;

  return files;
}

struct DataSet {
  std::string name;
  std::filesystem::path path;
};

Double_t get_genEventSumw(const std::string &file_path) {
  TFile *file = TFile::Open(file_path.c_str(), "READ");

  TTree *runs = (TTree *)file->Get("Runs");
  // TTree *events = (TTree *)file->Get("Events");

  Double_t genEventSumw(0), eventSumw;
  Long64_t genEventCount(0), eventCount;

  runs->SetBranchAddress("genEventSumw", &eventSumw);
  runs->SetBranchAddress("genEventCount", &eventCount);

  for (int iRun = 0; iRun < runs->GetEntries(); iRun++) {
    runs->GetEntry(iRun);
    genEventSumw += eventSumw;
    genEventCount += eventCount;
  }

  file->Close();
  return genEventSumw;
};

void fillHistos(DataSet sample, TFile &outFile) {
  ROOT::RDF::RNode df = ROOT::RDataFrame("Events", sample.path.c_str());

  df = df.Filter([](Short_t bestIdx) { return bestIdx >= 0; }, {"bestCandIdx"});

  std::vector<ROOT::RDF::RResultPtr<TH1D>> histograms;

  std::vector<ROOT::RDF::TH1DModel> massModels = {
      ROOT::RDF::TH1DModel(
          ("ZZMass_2GeV_" + sample.name).c_str(),
          ("ZZMass_2GeV_" + sample.name + ";m_{#it{4l}};Events / 2 Gev")
              .c_str(),
          65, 70., 200.),
      ROOT::RDF::TH1DModel(
          ("ZZMass_4GeV_" + sample.name).c_str(),
          ("ZZMass_4GeV_" + sample.name + ";m_{#it{4l}};Events / 4 Gev")
              .c_str(),
          233, 70., 1002.),
      ROOT::RDF::TH1DModel(
          ("ZZMass_10GeV_" + sample.name).c_str(),
          ("ZZMass_10GeV_" + sample.name + ";m_{#it{4l}};Events / 10 Gev")
              .c_str(),
          93, 70., 1000.)};

  for (auto model : massModels) {
    auto hist = df.Histo1D(model, "ZZCand_mass[bestCandIdx]");
    histograms.push_back(hist);
  }

  outFile.cd();
  for (auto &hist : histograms)
    hist->Write();
}

void runMC(const std::string &period = "2025") {
  std::string outName = "H4l_MC.root";
  auto outFile =
      std::unique_ptr<TFile>(TFile::Open(outName.c_str(), "RECREATE"));

  DataSet samples[sizeof(MC_dataSets)];

  for (size_t i = 0; i < MC_dataSets->size(); i++) {
    samples[i] =
        DataSet{.name = MC_dataSets[i],
                .path = std::filesystem::path(
                    MC_PATHS[period] + MC_dataSets[i] + "/ZZ4lAnalysis.root")};
  }

  for (auto &sample : samples) {
    fillHistos(sample, *outFile);

    for (auto histo : histos) {
      outFile->WriteObject(histo, histo.GetName());
    }
  }
}

void H4l_Fill(const std::vector<std::string> &years = {"2025"},
              int nEvents = -1) {
  runMC();
  return;
}
