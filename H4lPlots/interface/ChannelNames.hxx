#ifndef CHANNELNAMES_HXX
#define CHANNELNAMES_HXX

#include <array>
#include <cstddef>

template <typename T, size_t LLen, size_t RLen>
constexpr std::array<T, LLen + RLen> concat(const std::array<T, LLen> &a,
                                            const std::array<T, RLen> &b) {
  std::array<T, LLen + RLen> out{};
  for (size_t i = 0; i < LLen; ++i)
    out[i] = a[i];
  for (size_t i = 0; i < RLen; ++i)
    out[LLen + i] = b[i];
  return out;
}

constexpr std::array<const char *, 6> MC_SignalH124 = {
    "ggH124", "VBFH124", "WplusH124", "WminusH124", "ZH124", "ttH124"};

constexpr std::array<const char *, 6> MC_SignalH124p5 = {
    "ggH124p5",     "VBFH124p5", "WplusH124p5",
    "WminusH124p5", "ZH124p5",   "ttH124p5"};

constexpr std::array<const char *, 6> MC_SignalH125 = {
    "ggH125", "VBFH125", "WplusH125", "WminusH125", "ZH125", "ttH125"};

constexpr std::array<const char *, 6> MC_SignalH125p5 = {
    "ggH125p5",     "VBFH125p5", "WplusH125p5",
    "WminusH125p5", "ZH125p5",   "ttH125p5"};

constexpr std::array<const char *, 6> MC_SignalH126 = {
    "ggH126", "VBFH126", "WplusH126", "WminusH126", "ZH126", "ttH126"};

constexpr std::array<const char *, 4> MC_BkgEW = {"WZto3LNu", "WWZ", "WZZ",
                                                  "ZZZ"};

constexpr std::array<const char *, 1> MC_BkgQQZZ = {"ZZTo4l"};

constexpr std::array<const char *, 6> MC_BkgGGTo = {
    "ggTo4e_Contin_MCFM701",      "ggTo2e2mu_Contin_MCFM701",
    "ggTo2e2tau_Contin_MCFM701",  "ggTo4mu_Contin_MCFM701",
    "ggTo2mu2tau_Contin_MCFM701", "ggTo4tau_Contin_MCFM701"};

constexpr std::array<const char *, 2> MC_BkgDY = {"DYJetsTo2E", "DYJetsTo2Mu"};

constexpr std::array<const char *, 3> MC_BkgTT = {"TTto2L2Nu", "TTWW", "TTZZ"};

// clang-format off
constexpr std::array<const char *, 46> MC_dataSets = {
    "DYJetsTo2E", "DYJetsTo2Mu", "ggH124", "ggH124p5", "ggH125",
    "ggH125p5", "ggH126", "ggTo2e2mu_Contin_MCFM701", "ggTo2e2tau_Contin_MCFM701",
    "ggTo2mu2tau_Contin_MCFM701", "ggTo4e_Contin_MCFM701", "ggTo4mu_Contin_MCFM701",
    "ggTo4tau_Contin_MCFM701", "ttH124", "ttH124p5", "ttH125",
    "ttH125p5", "ttH126", "TTto2L2Nu", "TTWW", "TTZZ", "VBFH124",
    "VBFH124p5", "VBFH125", "VBFH125p5", "VBFH126", "WminusH124",
    "WminusH124p5", "WminusH125", "WminusH125p5", "WminusH126",
    "WplusH124", "WplusH124p5", "WplusH125", "WplusH125p5",
    "WplusH126", "WWZ", "WZto3LNu", "WZZ", "ZH124", "ZH124p5",
    "ZH125", "ZH125p5", "ZH126", "ZZTo4l", "ZZZ"
};
// clang-format on
constexpr std::array<const char *, 14> Data_EGamma2025 = {
    "EGamma02025Cv1", "EGamma02025Cv2", "EGamma02025Dv1", "EGamma02025Ev1",
    "EGamma02025Fv1", "EGamma02025Fv2", "EGamma02025Gv1", "EGamma12025Cv1",
    "EGamma12025Cv2", "EGamma12025Dv1", "EGamma12025Ev1", "EGamma12025Fv1",
    "EGamma12025Fv2", "EGamma12025Gv1"};

constexpr std::array<const char *, 14> Data_Muon2025 = {
    "Muon02025Cv1", "Muon02025Cv2", "Muon02025Dv1", "Muon02025Ev1",
    "Muon02025Fv1", "Muon02025Fv2", "Muon02025Gv1", "Muon12025Cv1",
    "Muon12025Cv2", "Muon12025Dv1", "Muon12025Ev1", "Muon12025Fv1",
    "Muon12025Fv2", "Muon12025Gv1"};

constexpr std::array<const char *, 7> Data_MuonEG2025 = {
    "MuonEG2025Cv1", "MuonEG2025Cv2", "MuonEG2025Dv1", "MuonEG2025Ev1",
    "MuonEG2025Fv1", "MuonEG2025Fv2", "MuonEG2025Gv1",
};

const std::array<const char *, 35> Data_dataSets = {
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

#endif
