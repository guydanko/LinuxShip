#include "../simulator/Simulator.h"
#include <filesystem>
#include <iostream>
#include "../common/FileHandler.h"
#include <unordered_map>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {

    std::error_code er;
    bool toRunSimulator = true;
    unordered_map<string, string> flagMap = {{"-travel_path",    ""},
                                             {"-output",         ""},
                                             {"-algorithm_path", ""},
                                             {"-num_threads",    ""}
    };

    string errorString = FileHandler::setCommandMap(flagMap, argv, argc);

    const string travelPath = flagMap["-travel_path"];
    string outPath = flagMap["-output"].empty() ? fs::current_path(er).string() : flagMap["-output"];
    const string algoPath = flagMap["-algorithm_path"].empty() ? fs::current_path(er).string()
                                                               : flagMap["-algorithm_path"];

    int numThreads;
    if (flagMap["-num_threads"].empty()) {
        numThreads = 1;
    } else {
        if (std::stoi(flagMap["-num_threads"]) > int(std::thread::hardware_concurrency())) {
            errorString += "Error: num_threads value (" + flagMap["-num_threads"] + ") larger than max thread value\n";
            numThreads= int(std::thread::hardware_concurrency());
        } else {
            numThreads = std::stoi(flagMap["-num_threads"]);
        }
    }

    if (!fs::exists(outPath, er)) {
        if (!fs::create_directories(outPath, er)) {
            errorString += "Error: could not create output directory, using current directory for output\n";
            outPath = fs::current_path(er).string();
            if(!FileHandler::canWriteinPath(outPath)) {
                errorString += "Fatal Error: user has no write permission to output path\n";
                toRunSimulator = false;
            }
        }
    } else {
        if (!FileHandler::canWriteinPath(outPath)) {
            errorString += "Fatal Error: user has no write permission to output path\n";
            toRunSimulator = false;
            outPath = fs::current_path(er).string();
        }
    }


    FileHandler::setUpErrorFiles(outPath);
    const string errorFilePath = outPath + "/errors/command.errors";
    std::ofstream errorFile(errorFilePath);

    errorFile << errorString;

    if (travelPath.empty()) {
        errorFile << "Fatal error: program must receive travel path\n";
        toRunSimulator = false;
    } else if (!fs::exists(travelPath, er)) {
        errorFile << "Fatal error: travelPath does not exist\n";
        toRunSimulator = false;
    }
    if (!fs::exists(algoPath, er)) {
        errorFile << "Fatal error: algorithm path does not exist\n";
        toRunSimulator = false;
    }
    if (!fs::exists(outPath, er)) {
        errorFile << "Error: output path does not exist\n";
    }

    if (!toRunSimulator) {
        errorFile.close();
        return EXIT_FAILURE;
    }

    errorFile.close();
    Simulator simulator(travelPath, algoPath, outPath, numThreads);
    simulator.run();
    return EXIT_SUCCESS;

}