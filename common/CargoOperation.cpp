#include "CargoOperation.h"

ostream& operator<<(ostream& os, const CargoOperation& cargoOperation){
    string type;
    switch (cargoOperation.operation){
        case AbstractAlgorithm::Action::MOVE:
            type = "M";
            break;
        case AbstractAlgorithm::Action::LOAD:
            type = "L";
            break;
        case AbstractAlgorithm::Action::REJECT:
            os << "R," << cargoOperation.container->getId();
            return os;
        case AbstractAlgorithm::Action::UNLOAD:
            type = "U";
            break;
    }
    os << type << "," << cargoOperation.container->getId() << ","   << cargoOperation.index.getHeight() << "," << cargoOperation.index.getRow() << "," << cargoOperation.index.getCol();
    if(cargoOperation.operation == AbstractAlgorithm::Action::MOVE){
        os << " [," << cargoOperation.moveIndex.getHeight() << "," << cargoOperation.moveIndex.getRow() << "," << cargoOperation.moveIndex.getCol() << "]";
    }
    return os;
}