/*Simulator class to run a simulation of cartesian loop of algorithms and travels*/
#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"
#include "MoreNaiveAlgorithm.h" //should erase
#include "SimulatorError.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using std::pair;

#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H


class Simulator {
    list<pair<shared_ptr<AbstractAlgorithm>, Travel>> algoXtravel;
    list<shared_ptr<AbstractAlgorithm>> algoList;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    string outputPath = "SimulatorFiles", travelPath, algoPath;

    void travelErrorsToFile(const string &fileName);

    void buildTravel(const fs::path &path);

    void deleteEmptyFiles();

    void createAlgoXTravel();

public:
    Simulator(const string &travelPath, const string &algoPath = fs::current_path().string(),
              const string &outputPath = fs::current_path().string()) : travelPath(
            travelPath),
                                                                        algoPath(
                                                                                algoPath),
                                                                        outputPath(
                                                                                outputPath) {};

    list<Travel> &getTravels() { return this->travelList; };

    void run();

    void runOneTravel(Travel &travel, shared_ptr<AbstractAlgorithm> algo, const string &fileName,
                      const string &errorFileName);
    bool initAlgoWithTravelParam(Travel &travel,shared_ptr<AbstractAlgorithm> pAlgo,list<SimulatorError>& listError);

};

#endif //SHIPGIT_SIMULATOR_H
