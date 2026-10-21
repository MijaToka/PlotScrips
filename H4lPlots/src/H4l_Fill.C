#pragma cling add_include_path(                                                \
    "/eos/user/m/mtokarev/work_area/currentCMSSW/src")

#include "PlotScripts/H4lPlots/interface/ChannelNames.hxx"
#include "PlotScripts/H4lPlots/interface/Paths.hxx"

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
#include <memory>
#include <string>
#include <vector>

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
          if (genEventSumw == 0 || bestIdx == -1)
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
