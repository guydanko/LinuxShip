#include "NaiveStowageAlgorithm.h"
#include "MoreNaiveAlgorithm.h"
#include "AbstractStowageAlgorithm.h"
#include "SimulatorError.h"
#include "Travel.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;


#ifndef SHIPGIT_SIMULATOR_H
#define SHIPGIT_SIMULATOR_H



class Simulator{
    list<AbstractStowageAlgorithm*> algoList;
    list<Travel> travelList;
    WeightBalanceCalculator calculator;
    string rootPath;
    void travelErrorsToFile(const string &fileName);
    void buildTravel(const fs::path& path);
public:
    Simulator(const string& simulationDirectory);
    list<Travel>& getTravels(){ return this->travelList;};
    ~Simulator();
    void run();
    void runOneTravel(Travel& travel, AbstractStowageAlgorithm *algo, const string& fileName);
    list<SimulatorError> checkAlgoCorrect(Ship &ship, list<CargoOperation> &cargoOpsList, list<Container *> &loadList, const string &currentPort);

};
#endif //SHIPGIT_SIMULATOR_H
