#include "Simulator.h"
#include <filesystem>
#include <iostream>
#include "map"

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
        } else{
            std::cerr << "Cannot run simulator, illegal flag - " << argv[i] << "!\n";
            return EXIT_FAILURE;
        }
    }

    if (argTravelPath.empty()) {
        std::cerr << "Cannot run simulator, program must receive travel path!\n";
        /*fatal error in file*/
        return EXIT_FAILURE;
    }
    if (!fs::exists(argTravelPath)) {
        std::cerr << "Cannot run simulator, travelPath does not exist!\n";
        /*fatal error in file*/
        return EXIT_FAILURE;
    }
    if (!argAlgoPath.empty() && !fs::exists(argAlgoPath)) {
        std::cerr << "Cannot run simulator, algorithm path does not exist!\n";
        /*fatal error in file*/
        return EXIT_FAILURE;
    }

    const string travelPath = argTravelPath;
    const string outPath = argOutputPath.empty() ? fs::current_path().string() : argOutputPath;
    const string algoPath = argAlgoPath.empty() ? fs::current_path().string() : argAlgoPath;

    Simulator simulator(travelPath, algoPath, outPath);
    simulator.run();
    return EXIT_SUCCESS;

}