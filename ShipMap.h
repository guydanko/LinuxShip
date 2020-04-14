
#include "Container.h"
#include <vector>
#include <set>
#include <list>
using std::vector;
using std::set;
#ifndef SHIPGIT_SHIPMAP_H
#define SHIPGIT_SHIPMAP_H

class ShipMap {

    const int height;
    const int rows;
    const int cols;
    Container *imaginaryContainer= nullptr;
    vector<vector<vector<Container*>>> shipMapContainer;
    set<string> containerIDOnShip= set<string>();

public:

    ShipMap(int height, int rows, int cols);

    ShipMap (const ShipMap &from); // copy only the structure not the containers

    void initShipMapContainer(int height, int rows, int cols);

    int getHeight()const{return this->height;}
    int getRows()const{return this->rows;}
    int getCols()const {return this->cols;}

    vector<vector<vector<Container*>>>& getShipMapContainer() { return this->shipMapContainer;}

    Container* getImaginary()const {return this->imaginaryContainer;}

    ShipMap& operator=(const ShipMap& other); // copy only the structure not the containers
    void clearContainers();
    set<string>& getContainerIDOnShip(){return containerIDOnShip;}

    ~ShipMap(){
        delete imaginaryContainer;
    }
};
#endif //SHIPGIT_SHIPMAP_H
