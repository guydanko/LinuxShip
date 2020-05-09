/*data structure to hold 3 index of ship location*/
#include "ShipMap.h"
#ifndef SHIPGIT_MAPINDEX_H
#define SHIPGIT_MAPINDEX_H

class MapIndex{
    int height;
    int row;
    int col;
public:
    MapIndex(int height=-1, int row=-1, int col=-1):height(height), row(row), col(col){}
    bool validIndex(){ return this->row != -1 && this->col != -1 && this->height != -1;}
    int getHeight()const{return this->height;}
    int getRow()const{return this->row;}
    int getCol()const {return this->col;}
    static MapIndex isPlaceToMove(MapIndex currentIndex, shared_ptr<ShipMap> ); //find empty legal place on the "first" side of the ship in regard to currentIndex
    static MapIndex firstLegalIndexPlace(shared_ptr<ShipMap> shipMap);
};


#endif //SHIPGIT_MAPINDEX_H
