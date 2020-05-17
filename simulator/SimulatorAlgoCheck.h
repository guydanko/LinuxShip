//Class to check that algorithm runs correctly
#include "SimulatorError.h"
#include "Travel.h"

#ifndef LINUXSHIP_SIMULATORALGOCHECK_H
#define LINUXSHIP_SIMULATORALGOCHECK_H

using std::list;

class SimulatorAlgoCheck{
public:
    static int checkAlgoCorrect(ShipMap* shipMap, list<string>& route, WeightBalanceCalculator& calculator, list<shared_ptr<CargoOperation>> &cargoOpsList, list<shared_ptr<Container>> &loadList, const string &currentPort, list<SimulatorError>& errorList, list<shared_ptr<Container>>& doubleIdList, bool& correctAlgo);
    static bool compareErrorAlgoSimulationInit(int algoInitError, int simulationInitError ,list<SimulatorError>& listError,bool& correctAlgo);

    static void algoErrorInstVsSimulationErrorInst(int algoGetInsError, int simulationInstError, list<SimulatorError>& errorList, bool& correctAlgo);

    static void checkIfShipEmpty(ShipMap* shipMap, list<SimulatorError> &errorList,bool& correctAlgo);
    static int connectContainerToCargoOp(list<shared_ptr<Container>> &loadList, ShipMap* shipMap,
                                                      list<shared_ptr<CargoOperation>> &cargoOps,
                                                      list<SimulatorError> &listError, list<shared_ptr<Container>> &doubleIdList, list<string>& route,set<string> &rejectedID,bool& correctAlgo);

};

#endif //LINUXSHIP_SIMULATORALGOCHECK_H
