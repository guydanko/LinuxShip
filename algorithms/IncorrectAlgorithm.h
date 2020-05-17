/* forget container on the port
 * return incorrect error code from get instructionForCargo
 * keep the second container if there are more than container with the same id in load list
 * load container with destination not in port or not current port*/


#include "../interfaces/AbstractAlgorithm.h"
#include "../common/MapIndex.h"
#include "../common/CargoOperation.h"
#include <memory>
#include "../interfaces/AlgorithmRegistration.h"
#include "AbstractCommonAlgorithm.h"

#ifndef LINUXSHIP_INNCORRECTALGORITHM_H
#define LINUXSHIP_INNCORRECTALGORITHM_H


#include "AbstractCommonAlgorithm.h"

class IncorrectAlgorithm : public AbstractCommonAlgorithm {

public:
    IncorrectAlgorithm() : AbstractCommonAlgorithm() {}

    int rejectAllBesideShipFull(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList,
                                const string &currentPort) override;
    int rejectDoubleId(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) override ;
    void
    moveTower(MapIndex index, const string &portName, list<shared_ptr<Container>>& rememberLoadAgain,
              list<CargoOperation> &opList) override ;
    void loadAgain(list<shared_ptr<Container>>& rememberLoadAgain, list<CargoOperation> &opList) override ;
};


#endif //LINUXSHIP_INNCORRECTALGORITHM_H
