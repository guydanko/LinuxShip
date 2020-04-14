
#include "MapIndex.h"

 MapIndex MapIndex::isPlaceToMove(MapIndex currentIndex, ShipMap& shipMap ){
    for(int i = 0; i < currentIndex.row; i++){
        for(int j = 0; j < shipMap.getCols(); j++){
            for (int k = 0; k < shipMap.getHeight(); k++) {
                if(shipMap.getShipMapContainer()[k][i][j] == nullptr){
                       return MapIndex(k ,i ,j );
                }
            }
        }
    }
    for(int j = 0; j < currentIndex.col; j++) {
        for (int k = 0; k < shipMap.getHeight(); k++) {
            if (shipMap.getShipMapContainer()[k][currentIndex.row][j] == nullptr) {
                return MapIndex(k, currentIndex.row, j);
            }
        }
    }
    return MapIndex();
}
MapIndex MapIndex::firstLegalIndexPlace(ShipMap& shipMap){
    for(int i = 0; i < shipMap.getRows(); i++){
        for(int j = 0; j < shipMap.getCols(); j++){
            for (int k = 0; k < shipMap.getHeight(); k++) {
                if(shipMap.getShipMapContainer()[k][i][j] == nullptr){
                    return MapIndex(k ,i ,j );
                }
            }
        }
    }
    return MapIndex();
}