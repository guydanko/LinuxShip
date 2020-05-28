#include "MapIndex.h"

MapIndex MapIndex::isPlaceToMove( ShipMap *shipMap, const string &currentPort) {
    bool moveToNextPlace;
    for (int i = 0; i < shipMap->getHeight(); i++) {
        for (int j = 0; j < shipMap->getCols(); j++) {
            moveToNextPlace = false;
            for (int k = 0; k < shipMap->getRows() && !moveToNextPlace; k++) {
                if (shipMap->getShipMapContainer()[i][k][j] == nullptr) {
                    return MapIndex(i, k, j);
                } else {
                    if (shipMap->getShipMapContainer()[i][k][j]->getDestination() == currentPort) {
                        moveToNextPlace = true;
                    }
                }
            }
        }
    }
//    for(int j = 0; j < currentIndex.col; j++) {
//        for (int k = 0; k < shipMap->getHeight(); k++) {
//            if (shipMap->getShipMapContainer()[k][currentIndex.row][j] == nullptr) {
//                return MapIndex(k, currentIndex.row, j);
//            }
//        }
//    }
    return MapIndex();
}

MapIndex MapIndex::firstLegalIndexPlace(ShipMap *shipMap) {
    for (int i = 0; i < shipMap->getHeight(); i++) {
        for (int j = 0; j < shipMap->getCols(); j++) {
            for (int k = 0; k < shipMap->getRows(); k++) {
                if (shipMap->getShipMapContainer()[i][k][j] == nullptr) {
                    return MapIndex(i, k, j);
                }
            }
        }
    }
    return MapIndex();
}