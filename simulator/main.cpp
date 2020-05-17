#include "../simulator/Simulator.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../common/FileHandler.h"
#include <unordered_map>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    bool toRunSimulator = true;
    unordered_map<string, string> flagMap = {{"-travel_path",    ""},
                                             {"-output",         ""},
                                             {"-algorithm_path", ""}};

    string errorString = FileHandler::setCommandMap(flagMap, argv, argc);

    const string travelPath = flagMap["-travel_path"];
    string outPath = flagMap["-output"].empty() ? fs::current_path().string() : flagMap["-output"];
    const string algoPath = flagMap["-algorithm_path"].empty() ? fs::current_path().string()
                                                               : flagMap["-algorithm_path"];
    if(!FileHandler::canWriteinPath(outPath)){
        errorString += "Fatal Error: user has no write permission to output path\n";
        toRunSimulator = false;
        outPath = fs::current_path().string();
    }

    FileHandler::setUpErrorFiles(outPath);
    const string errorFilePath = outPath + "/errors/command.errors";

    std::ofstream errorFile(errorFilePath);
    errorFile << errorString;

    if (travelPath.empty()) {
        errorFile << "Fatal error: program must receive travel path\n";
        toRunSimulator = false;
    } else if (!fs::exists(travelPath)) {
        errorFile << "Fatal error: travelPath does not exist\n";
        toRunSimulator = false;
    }
    if (!fs::exists(algoPath)) {
        errorFile << "Fatal error: algorithm path does not exist\n";
        toRunSimulator = false;
    }
    if (!fs::exists(outPath)) {
        errorFile << "Error: output path does not exist\n";
    }

    if (!toRunSimulator) {
        errorFile.close();
        return EXIT_FAILURE;
    }

    errorFile.close();
    Simulator simulator(travelPath, algoPath, outPath);
    simulator.runWindows();
    return EXIT_SUCCESS;

}