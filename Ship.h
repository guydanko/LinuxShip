/*Data structure that represents a ship object (plan + route)*/
#include <string>
#include "ShipMap.h"

using std::string;
using std::list;

#ifndef SHIPGIT_SHIP_H
#define SHIPGIT_SHIP_H


class Ship {
    list<string> shipRoute;
    ShipMap shipMap;

public:

    Ship(int height, int rows, int cols) : shipMap(height, rows, cols) {}

    ShipMap &getShipMap() { return this->shipMap; }

    list<string> &getShipRoute() { return this->shipRoute; }

    void setShipRoute(list<string> routes) { this->shipRoute = routes; };

    const string &getCurrentPort() const { return this->shipRoute.front(); };

    void sailToNextPort();

    ~Ship() {};


};

#endif //SHIPGIT_SHIP_H
