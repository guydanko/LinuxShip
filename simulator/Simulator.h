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
    Producer producer;
    std::vector<std::thread> workers;
    unordered_map<string, unordered_map<string, int>> resultMap;

    void travelErrorsToFile(const string &fileName);

    void buildTravel(const fs::path &path);

    void deleteEmptyFiles();

    void createAlgoXTravel();

    void cleanFiles(list<string> &algoNames);

    void printResults(unordered_map<string, unordered_map<string, int>> simResults);

    void createAlgoFactory();

    void setUpFakeFile();
    void  initializeWorkers();
    void workerFunction();
    void waitTillFinish();

public:
    Simulator(const string &travelPath, const string &algoPath, const string &outputPath, int numTasks) : travelPath(travelPath),
                                                                                            algoPath(algoPath),
                                                                                            outputPath(outputPath), producer(numTasks) {};
    void run();

    int runOneTravel(Travel &travel, AbstractAlgorithm *algo, const string &fileName,
                     const string &errorFileName);

    int initAlgoWithTravelParam(Travel &travel, AbstractAlgorithm *pAlgo, list<SimulatorError> &errorList,
                                bool &correctAlgo);
    friend class Producer;

};

#endif //SHIPGIT_SIMULATOR_H
