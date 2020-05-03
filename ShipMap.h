/*Data structure that holds the map of a specific ship*/
#include "Container.h"
#include <vector>
#include <set>
#include <list>
#include <memory>

using std::shared_ptr;
using std::vector;
using std::set;

#ifndef SHIPGIT_SHIPMAP_H
#define SHIPGIT_SHIPMAP_H

class ShipMap {

    int height;
    int rows;
    int cols;
    shared_ptr<Container> imaginaryContainer = nullptr;
    vector<vector<vector<shared_ptr<Container>>>> shipMapContainer;
    set<string> containerIDOnShip = set<string>();

public:

    ShipMap(int height = 0, int rows = 0, int cols = 0);

    ShipMap(const ShipMap &from); // copy only the structure not the containers

    void initShipMapContainer(int height, int rows, int cols);

    int getHeight() const { return this->height; }

    int getRows() const { return this->rows; }

    int getCols() const { return this->cols; }

    vector<vector<vector<shared_ptr<Container>>>> &getShipMapContainer() { return this->shipMapContainer; }

    shared_ptr<Container> getImaginary() const { return this->imaginaryContainer; }

    ShipMap &operator=(const ShipMap &other); // copy only the structure not the containers
    void clearContainers();

    set<string> &getContainerIDOnShip() { return containerIDOnShip; }

};

#endif //SHIPGIT_SHIPMAP_H
