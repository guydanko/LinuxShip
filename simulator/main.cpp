#include "Simulator.h"
#include <filesystem>
#include <iostream>
#include "map"
#include <fstream>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    const string travelFlag = "-travel_path", algoFlag = "-algorithm_path", outputFlag = "-output";
    string argTravelPath = "", argAlgoPath = "", argOutputPath = "";

    for (int i = 1; i < argc; i += 2) {
        if (0 == travelFlag.compare(argv[i])) {
            argTravelPath = argc >= i + 2 ? argv[i + 1] : "";
        } else if (0 == algoFlag.compare(argv[i])) {
            argAlgoPath = argc >= i + 2 ? argv[i + 1] : "";
        } else if (0 == outputFlag.compare(argv[i])) {
            argOutputPath = argc >= i + 2 ? argv[i + 1] : "";
        } else {
            std::cerr << "Cannot run simulator, illegal flag - " << argv[i] << "!\n";
            return EXIT_FAILURE;
        }
    }

    const string travelPath = argTravelPath;
    const string outPath = argOutputPath.empty() ? fs::current_path().string() : argOutputPath;
    const string algoPath = argAlgoPath.empty() ? fs::current_path().string() : argAlgoPath;

    string errorFilePath;

    if (!fs::exists(outPath)) {
        errorFilePath = fs::current_path().string() + "/simulator.errors";
    } else {
        errorFilePath = outPath + "/simulator.errors";
    }


    std::ofstream errorFile(errorFilePath);
    bool toRunSimulator = true;

    if (argTravelPath.empty()) {
        errorFile << "Fatal error: program must receive travel path!\n";
        toRunSimulator = false;
    }
    if (!fs::exists(argTravelPath)) {
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