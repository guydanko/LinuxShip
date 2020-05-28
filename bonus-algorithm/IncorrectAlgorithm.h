//IncorrectAlgorithm - that consist of multiple errors:
//1) Can unload containers in wrong destination port
//2) Returns incorrect error code from get instructionForCargo
//3) Loads the second container (and not the first!) if there are more than one container with the same id in load list
//4) Loads containers with destination that does not appear in ship route, or that its ports destination is the current port

#include "../interfaces/AbstractAlgorithm.h"
#include "../common/MapIndex.h"
#include "../common/CargoOperation.h"
#include <memory>
#include "../interfaces/AlgorithmRegistration.h"
#include "../algorithm/AbstractCommonAlgorithm.h"

#ifndef LINUXSHIP_INNCORRECTALGORITHM_H
#define LINUXSHIP_INNCORRECTALGORITHM_H


#include "../algorithm/AbstractCommonAlgorithm.h"

class IncorrectAlgorithm : public AbstractCommonAlgorithm {

public:
    IncorrectAlgorithm() : AbstractCommonAlgorithm() {}

    int rejectAllBesideShipFull(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList,
                                const string &currentPort) override;
    int rejectDoubleId(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) override ;
    void
    moveTower(MapIndex index, const string &portName, list<shared_ptr<Container>>& rememberLoadAgain,
              list<CargoOperation> &opList) override ;

};


#endif //LINUXSHIP_INNCORRECTALGORITHM_H
