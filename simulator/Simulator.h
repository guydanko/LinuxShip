/*Simulator class to run a simulation of cartesian loop of algorithms and travels*/
#include "../interfaces/AbstractAlgorithm.h"
#include "../algorithms/_316294636_b.h" //should erase
#include "SimulatorError.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;
using std::pair;
using std::unique_ptr;

#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H


class Simulator {
    list<unique_ptr<AbstractAlgorithm>> algoList;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    string travelPath, algoPath, outputPath = "SimulatorFiles";

    void travelErrorsToFile(const string &fileName);

    void buildTravel(const fs::path &path);

    void deleteEmptyFiles();

    void createAlgoXTravel();

    void printResults(unordered_map<string, unordered_map<string, int>> simResults);

public:
    Simulator(const string &travelPath, const string &algoPath = fs::current_path().string(),
              const string &outputPath = fs::current_path().string() + "/SimulatorFiles") : travelPath(
            travelPath),
                                                                                            algoPath(
                                                                                                    algoPath),
                                                                                            outputPath(
                                                                                                    outputPath) {};

    list<Travel> &getTravels() { return this->travelList; };

    void run();

    int runOneTravel(Travel &travel, AbstractAlgorithm *algo, const string &fileName,
                     const string &errorFileName);

    int initAlgoWithTravelParam(Travel &travel, AbstractAlgorithm *pAlgo);

};

#endif //SHIPGIT_SIMULATOR_H
