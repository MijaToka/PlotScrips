#include <ROOT/RDF/HistoModels.hxx>
#include <ROOT/RDataFrame.hxx>
#include <ROOT/RResultPtr.hxx>
#include <ROOT/RVec.hxx>
#include <RtypesCore.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TString.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <cstdio>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

std::map<std::string, std::string> MC_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/MC_2025_d25e5b4/"},
};
std::map<std::string, std::string> DATA_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/Data_2025_d25e5b4/"},
};
// clang-format off
const std::vector<std::string> MC_dataSets = {
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
const std::vector<std::string> Data_dataSets = {
    "EGamma02025Cv1", "EGamma02025Cv2", "EGamma02025Dv1", "EGamma02025Ev1",
    "EGamma02025Fv1", "EGamma02025Fv2", "EGamma02025Gv1", "EGamma12025Cv1",
    "EGamma12025Cv2", "EGamma12025Dv1", "EGamma12025Ev1", "EGamma12025Fv1",
    "EGamma12025Fv2", "EGamma12025Gv1", "Muon02025Cv1",   "Muon02025Cv2",
    "Muon02025Dv1",   "Muon02025Ev1",   "Muon02025Fv1",   "Muon02025Fv2",
    "Muon02025Gv1",   "Muon12025Cv1",   "Muon12025Cv2",   "Muon12025Dv1",
    "Muon12025Ev1",   "Muon12025Fv1",   "Muon12025Fv2",   "Muon12025Gv1",
    "MuonEG2025Cv1",  "MuonEG2025Cv2",  "MuonEG2025Dv1",  "MuonEG2025Ev1",
    "MuonEG2025Fv1",  "MuonEG2025Fv2",  "MuonEG2025Gv1",
};

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

void fillHistos(const DataSet &sample, TFile &outFile, const bool &isMC) {

  std::vector<std::string> files;
  Float_t genEventSumw(0);
  for (auto const &file : std::filesystem::directory_iterator{sample.path}) {
    if (file.path().extension() == ".root" && file.is_regular_file()) {
      files.push_back(file.path().string());

      if (isMC) {

        Float_t currentEventSumw = get_genEventSumw(file.path().string());

        printf("\tAdding file %s with eventSumw=%f\n", file.path().c_str(),
               currentEventSumw);

        genEventSumw += currentEventSumw;
      } else {
        printf("\tAdding file %s\n", file.path().c_str());
      }
    }
  }

  ROOT::RDF::RNode df = ROOT::RDataFrame("Events", files);

  df = df.Filter(
             [](Short_t bestIdx, Bool_t HLT_passZZ4l) {
               return bestIdx >= 0 && HLT_passZZ4l;
             },
             {"bestCandIdx", "HLT_passZZ4l"})
           .Define("ZZCand_mass_best",
                   [](Short_t bestIdx, ROOT::RVecF mass) -> Float_t {
                     return mass[bestIdx];
                   },
                   {"bestCandIdx", "ZZCand_mass"})
           .Define("eventWeight", []() { return 1.; }, {});

  if (isMC) {
    df = df.Redefine(
        "eventWeight",
        [genEventSumw](Float_t overallWeight, ROOT::RVecF dataMCWeight,
                       Short_t bestIdx) -> Float_t {
          if (genEventSumw == 0)
            return 0.;
          else if (bestIdx <= 0)
            return 0.;
          else
            return overallWeight * dataMCWeight[bestIdx] / genEventSumw;
        },
        {"overallEventWeight", "ZZCand_dataMCWeight", "bestCandIdx"});
  }

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
    auto hist = df.Histo1D(model, "ZZCand_mass_best", "eventWeight");
    histograms.push_back(hist);
  }

  outFile.cd();
  for (auto &hist : histograms)
    hist->Write();
}

void run(const bool isMC, const std::string &period = "2025") {
  TString outName = TString::Format("H4lPlots_%s%s.root", isMC ? "MC" : "Data",
                                    period.c_str());
  std::vector<std::string> dataSets = isMC ? MC_dataSets : Data_dataSets;
  auto INPUT_PATH = isMC ? MC_PATHS : DATA_PATHS;

  auto outFile = std::unique_ptr<TFile>(TFile::Open(outName, "RECREATE"));

  printf("Output will be printed to %s\n", outName.Data());

  for (std::string channel : dataSets) {
    DataSet sample =
        DataSet{.name = channel,
                .path = std::filesystem::path(INPUT_PATH[period] + channel)};

    printf("Filling histograms from ROOT file(s) in %s \n",
           sample.path.c_str());

    fillHistos(sample, *outFile, isMC);
  }
  printf("Closing file %s\n", outName.Data());
  outFile->Close();
}

void runMC(const std::string &period = "2025") { run(true, period); }

void runData(const std::string &period = "2025") { run(false, period); }

void H4l_Fill(const std::vector<std::string> &years = {"2025"},
              int nEvents = -1) {
  runMC();
  runData();
  return;
}
