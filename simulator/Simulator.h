/*Simulator class to run a simulation of cartesian loop of algorithm and travels*/
#include "../interfaces/AbstractAlgorithm.h"
#include "SimulatorError.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>
#include <memory>

namespace fs = std::filesystem;
using std::pair;
using std::unique_ptr;
using std::vector;

#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H


class Simulator {
    list<unique_ptr<AbstractAlgorithm>> algoList;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    const string travelPath, algoPath, outputPath;

    void travelErrorsToFile(const string &fileName);

    void buildTravel(const fs::path &path);

    void deleteEmptyFiles();

    void createAlgoXTravel();

    void cleanFiles(list<string> &algoNames);

    void printResults(unordered_map<string, unordered_map<string, int>> simResults);

    void createAlgoList();

    void setUpFakeFile();

public:
    Simulator(const string &travelPath, const string &algoPath, const string &outputPath) : travelPath(travelPath),
                                                                                            algoPath(algoPath),
                                                                                            outputPath(outputPath) {};
    void run();

    int runOneTravel(Travel &travel, AbstractAlgorithm *algo, const string &fileName,
                     const string &errorFileName);

    int initAlgoWithTravelParam(Travel &travel, AbstractAlgorithm *pAlgo, list<SimulatorError> &errorList,
                                bool &correctAlgo);

};

#endif //SHIPGIT_SIMULATOR_H
