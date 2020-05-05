#include <memory>
#include "AbstractAlgorithm.h"
#include "Travel.h"

using std::shared_ptr;

#ifndef LINUXSHIP_ALGOTRAVELPAIR_H
#define LINUXSHIP_ALGOTRAVELPAIR_H

#endif //LINUXSHIP_ALGOTRAVELPAIR_H

class AlgoTravelPair {

    shared_ptr<AbstractAlgorithm> algo;
    shared_ptr<Travel> travel;
    int sumCargoInstructions = 0, sumErrors = 0;

    AlgoTravelPair(shared_ptr<AbstractAlgorithm> algo, shared_ptr<Travel> travel):algo(algo),travel(travel){};

    void incrementInstructions(int instructionSum){this->sumCargoInstructions += instructionSum;};
    void incrementErrors(int errorAmount){this->sumErrors += errorAmount;};


};