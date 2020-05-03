#include "SimulatorError.h"

#ifndef LINUXSHIP_SIMULATORALGOCHECK_H
#define LINUXSHIP_SIMULATORALGOCHECK_H

#endif //LINUXSHIP_SIMULATORALGOCHECK_H

using std::list;

class SimulatorAlgoCheck{
public:
    static list<SimulatorError> checkAlgoCorrect(shared_ptr<ShipMap> shipMap, list<string>& route,WeightBalanceCalculator& calculator,list<shared_ptr<CargoOperation>> &cargoOpsList, list<shared_ptr<Container>> &loadList, const string &currentPort, int& numberLoads, int& numberUnloads,list<SimulatorError>& listError,list<shared_ptr<Container>>& doubleIdList);
};
