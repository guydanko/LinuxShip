#include <iostream>
#include "Container.h"
#include "MapIndex.h"

using std::ostream ;
#ifndef SHIPGIT_CARGOOPERATION_H
#define SHIPGIT_CARGOOPERATION_H

enum class RejectReason {
    PORT_DEST_DOESNT_EXIST, CONT_PARAM_ILLEGAL, SHIP_FULL, CONT_ID_TAKEN, ALL_GOOD, CONT_DEST_IS_HERE
};
enum class Operation {
    LOAD, UNLOAD, REJECT, MOVE
};
class CargoOperation {
    Operation operation;
    Container* container;
    MapIndex index;
    MapIndex moveIndex;

    int placeInList = 0;
public:
    CargoOperation(){}

    CargoOperation(Operation op, Container* container,MapIndex index, MapIndex moveIndex={-1,1,-1}): operation(op), container(container), index(index), moveIndex(moveIndex) {};

    friend ostream& operator<<(ostream& os, const CargoOperation& cargoOperation);

    const Operation& getOp()const { return this->operation;}

    MapIndex getIndex()const{ return this->index;}

    MapIndex getMoveIndex()const{ return this->moveIndex;}

    Container* getContainer(){return this->container;}

    int getPlaceInList() const{ return this->placeInList;}

    void setPlaceInList(int i){this->placeInList=i;}

    /*//prepare option to work with RejectReason
    RejectReason getRejectReason(){ return this->reason;}*/
};

#endif //SHIPGIT_CARGOOPERATION_H