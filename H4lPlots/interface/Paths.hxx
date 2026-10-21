#ifndef PATHS_HXX
#define PATHS_HXX

#include <filesystem>
#include <map>
#include <string>

const std::map<std::string, std::string> MC_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/MC_2025_d25e5b4/"},
};
const std::map<std::string, std::string> DATA_PATHS = {
    {"2025", "/eos/user/m/mtokarev/samplesHZZ/Data_2025_d25e5b4/"},
};

const std::filesystem::path
    REPO_PATH("/eos/user/m/mtokarev/work_area/currentCMSSW/src/");

#endif
