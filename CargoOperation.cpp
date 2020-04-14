#include "CargoOperation.h"

ostream& operator<<(ostream& os, const CargoOperation& cargoOperation){
    string type;
    switch (cargoOperation.operation){
        case Operation::MOVE:
            type = "M";
            break;
        case Operation::LOAD:
            type = "L";
            break;
        case Operation::REJECT:
            os << "R," << cargoOperation.container->getId();
            return os;
        case Operation::UNLOAD:
            type = "U";
            break;
    }
    os << type << "," << cargoOperation.container->getId() << "," << cargoOperation.index.getHeight() << "," << cargoOperation.index.getRow() << "," << cargoOperation.index.getCol();
    if(cargoOperation.operation == Operation::MOVE){
        os << " [" << cargoOperation.moveIndex.getHeight() << "," << cargoOperation.moveIndex.getRow() << "," << cargoOperation.moveIndex.getCol() << "]";
    }
    return os;
}