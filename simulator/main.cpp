#include "Simulator.h"
#include <filesystem>
#include <iostream>
#include "map"
#include <fstream>
#include "../common/FileHandler.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    const string travelFlag = "-travel_path", algoFlag = "-algorithm_path", outputFlag = "-output";
    string argTravelPath = "", argAlgoPath = "", argOutputPath = "", errorString = "";

    for (int i = 1; i < argc; i += 2) {
        if (0 == travelFlag.compare(argv[i])) {
            argTravelPath = argc >= i + 2 ? argv[i + 1] : "";
        } else if (0 == algoFlag.compare(argv[i])) {
            argAlgoPath = argc >= i + 2 ? argv[i + 1] : "";
        } else if (0 == outputFlag.compare(argv[i])) {
            argOutputPath = argc >= i + 2 ? argv[i + 1] : "";
        } else {
            errorString += "Fatal Error: illegal flag - " + std::string(argv[i]) + "!\n";
        }
    }


    const string travelPath = argTravelPath;
    const string outPath = argOutputPath.empty() ? fs::current_path().string() : argOutputPath;
    const string algoPath = argAlgoPath.empty() ? fs::current_path().string() : argAlgoPath;


    FileHandler::setUpErrorFiles(outPath);
    const string errorFilePath = fs::exists(outPath) ? outPath + "/errors/command.errors" :
                                 fs::current_path().string() + "/errors/command.errors";

    std::ofstream errorFile(errorFilePath);
    errorFile << errorString;
    bool toRunSimulator = errorString.empty() ? true : false;

    if (argTravelPath.empty()) {
        errorFile << "Fatal error: program must receive travel path!\n";
        toRunSimulator = false;
    } else if (!fs::exists(argTravelPath)) {
        errorFile << "Fatal error: travelPath does not exist!\n";
        toRunSimulator = false;
    }
    if (!argAlgoPath.empty() && !fs::exists(argAlgoPath)) {
        errorFile << "Fatal error: algorithm path does not exist!\n";
        toRunSimulator = false;
    }
    if (!fs::exists(outPath)) {
        errorFile << "Fatal error: output path does not exist!\n";
        toRunSimulator = false;
    }

    if (!toRunSimulator) {
        errorFile.close();
        return EXIT_FAILURE;
    }

    Simulator simulator(travelPath, algoPath, outPath);
    simulator.run();
    errorFile.close();
    return EXIT_SUCCESS;

}