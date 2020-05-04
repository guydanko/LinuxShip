/*Data structure that represents a crane operation*/
#include <iostream>
#include "Container.h"
#include "MapIndex.h"
#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"
#include <memory>

using std::ostream;
using std::shared_ptr;
#ifndef SHIPGIT_CARGOOPERATION_H
#define SHIPGIT_CARGOOPERATION_H


class CargoOperation {
    AbstractAlgorithm::Action operation;
    shared_ptr<Container> container;
    MapIndex index;
    MapIndex moveIndex;

public:
    CargoOperation() {}

    CargoOperation(AbstractAlgorithm::Action op, shared_ptr<Container> container, MapIndex index = {-1, -1, -1},
                   MapIndex moveIndex = {-1, 1, -1}) : operation(op), container(container), index(index),
                                                       moveIndex(moveIndex) {};

    friend ostream &operator<<(ostream &os, const CargoOperation &cargoOperation);

    const AbstractAlgorithm::Action &getOp() const { return this->operation; }

    MapIndex getIndex() const { return this->index; }

    MapIndex getMoveIndex() const { return this->moveIndex; }

    shared_ptr<Container> getContainer() { return this->container; }

    void setContainer(shared_ptr<Container> cont){ this->container=cont;}

};

#endif //SHIPGIT_CARGOOPERATION_H