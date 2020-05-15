#include "Simulator.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../common/FileHandler.h"
#include <unordered_map>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    unordered_map<string, string> flagMap = {{"-travel_path",    ""},
                                             {"-output",         ""},
                                             {"-algorithm_path", ""}};

    string errorString = FileHandler::setCommandMap(flagMap, argv, argc);

    const string travelPath = flagMap["-travel_path"];
    const string outPath = flagMap["-output"].empty() ? fs::current_path().string() : flagMap["-output"];
    const string algoPath = flagMap["-algorithm_path"].empty() ? fs::current_path().string()
                                                               : flagMap["-algorithm_path"];

    FileHandler::setUpErrorFiles(outPath);
    const string errorFilePath = outPath + "/errors/command.errors";

    std::ofstream errorFile(errorFilePath);
    errorFile << errorString;
    bool toRunSimulator = true;

    if (travelPath.empty()) {
        errorFile << "Fatal error: program must receive travel path!\n";
        toRunSimulator = false;
    } else if (!fs::exists(travelPath)) {
        errorFile << "Fatal error: travelPath does not exist!\n";
        toRunSimulator = false;
    }
    if (!fs::exists(algoPath)) {
        errorFile << "Fatal error: algorithm path does not exist!\n";
        toRunSimulator = false;
    }
    if (!fs::exists(outPath)) {
        errorFile << "Error: output path does not exist!\n";
    }

    if (!toRunSimulator) {
        errorFile.close();
        return EXIT_FAILURE;
    }

    errorFile.close();
    Simulator simulator(travelPath, algoPath, outPath);
    simulator.run();
    return EXIT_SUCCESS;

}