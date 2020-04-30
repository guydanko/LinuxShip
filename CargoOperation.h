/*Data structure that represents a crane operation*/
#include <iostream>
#include "Container.h"
#include "MapIndex.h"
#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"

using std::ostream ;
#ifndef SHIPGIT_CARGOOPERATION_H
#define SHIPGIT_CARGOOPERATION_H



class CargoOperation {
    AbstractAlgorithm::Action operation;
    Container* container;
    MapIndex index;
    MapIndex moveIndex;

    int placeInList = 0;
public:
    CargoOperation(){}

    CargoOperation(AbstractAlgorithm::Action op, Container* container,MapIndex index, MapIndex moveIndex={-1,1,-1}): operation(op), container(container), index(index), moveIndex(moveIndex) {};

    friend ostream& operator<<(ostream& os, const CargoOperation& cargoOperation);

    const AbstractAlgorithm::Action& getOp()const { return this->operation;}

    MapIndex getIndex()const{ return this->index;}

    MapIndex getMoveIndex()const{ return this->moveIndex;}

    Container* getContainer(){return this->container;}

    int getPlaceInList() const{ return this->placeInList;}

    void setPlaceInList(int i){this->placeInList=i;}

    /*//prepare option to work with RejectReason
    RejectReason getRejectReason(){ return this->reason;}*/
};

#endif //SHIPGIT_CARGOOPERATION_H