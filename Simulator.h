/*Simulator class to run a simulation of cartesian loop of algorithms and travels*/
#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"
#include "MoreNaiveAlgorithm.h" //should erase
#include "IncorrectAlgorithm.h" //should erase
#include "SimulatorError.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H



class Simulator{
    list<AbstractAlgorithm*> algoList;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    string rootPath;
    void travelErrorsToFile(const string &fileName);
    void buildTravel(const fs::path& path);
    static void deleteEmptyFiles();

public:
    Simulator(const string& simulationDirectory);
    list<Travel>& getTravels(){ return this->travelList;};
    ~Simulator();
    void run();
    void runOneTravel(Travel& travel, AbstractAlgorithm *algo, const string& fileName);
    list<SimulatorError> checkAlgoCorrect(Ship *ship, list<shared_ptr<CargoOperation>> &cargoOpsList, list<shared_ptr<Container>> &loadList, const string &currentPort, int& numberLoads, int& numberUnloads,list<SimulatorError>& listError);

};
#endif //SHIPGIT_SIMULATOR_H
