/*Simulator class to run a simulation of cartesian loop of algorithm and travels*/
#include "../interfaces/AbstractAlgorithm.h"
#include "../interfaces/WeightBalanceCalculator.h"
#include "SimulatorError.h"
#include "AlgorithmRegistrar.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include "../common/FileHandler.h"
#include "SimulatorAlgoCheck.h"
#include "Producer.h"
#include <map>
#include <fstream>
#include <thread>

namespace fs = std::filesystem;
using std::pair;
using std::unique_ptr;
using std::vector;
using std::string;
using std::multimap;

#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H


class Simulator {
    list<std::function<std::unique_ptr<AbstractAlgorithm>()>> algoFactory;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    const string travelPath, algoPath, outputPath;
    std::vector<std::thread> workers;
    unordered_map<string, unordered_map<string, int>> resultMap;
    int numThreads;

    void travelErrorsToFile(const string &fileName);

    void buildTravel(const fs::path &path);

    void deleteEmptyFiles();

    void createAlgoXTravel();

    void cleanFiles(list<string> &algoNames);

    void printResults(unordered_map<string, unordered_map<string, int>> simResults);

    void createAlgoFactory();

    void setUpFakeFile();

    void initializeWorkers(Producer& producer);

    void workerFunction(Producer& producer);

    void waitTillFinish();

    void runOnlyMain(list<string>& algoNames);

public:
    Simulator(const string &travelPath, const string &algoPath, const string &outputPath, int numThreads) : travelPath(
            travelPath),
                                                                                                            algoPath(
                                                                                                                    algoPath),
                                                                                                            outputPath(
                                                                                                                    outputPath),
                                                                                                            numThreads(
                                                                                                                    numThreads) {};

    void run();

    int runOneTravel(Travel travel, std::unique_ptr<AbstractAlgorithm> pAlgo, const string &fileName,
                     const string &errorFileName);

    int
    initAlgoWithTravelParam(Travel &travel, std::unique_ptr<AbstractAlgorithm> &pAlgo, list<SimulatorError> &errorList,
                            bool &correctAlgo);

    friend class Producer;

};

#endif //SHIPGIT_SIMULATOR_H
