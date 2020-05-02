#include <memory>
#include "AbstractAlgorithm.h"
#include "Travel.h"

using std::shared_ptr;

#ifndef LINUXSHIP_ALGOTRAVELPAIR_H
#define LINUXSHIP_ALGOTRAVELPAIR_H

#endif //LINUXSHIP_ALGOTRAVELPAIR_H

class AlgoTravelPair{

    shared_ptr<AbstractAlgorithm> algo;
    shared_ptr<Travel> travel;
    int sumCargoInstructions;
    int sumErrors;

};