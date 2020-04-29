/*Data structure to control flow of a specific travel of a ship via ports*/
#ifndef SHIPGIT_TRAVEL_H
#define SHIPGIT_TRAVEL_H

#include <unordered_map>
#include <iostream>
#include "Container.h"
#include "Ship.h"

using std::tuple;
using std::string;
using std::unordered_map;
using std::list;
using std::get;

class Travel {
    unordered_map<string, tuple<int, int>> portCounter;
    Ship *ship;
    list<string> originalRoute;
    string travelPath;
    string travelName;
public:

    Travel(const string &travelPath, const string &travelName, Ship *ship);

    Ship *getShip() { return this->ship; }

    string getTravelName() const;

    list<Container *> getContainerList(const string &errorFile);

    void goToNextPort();

    void setToOriginalTravel();

    bool didTravelEnd() { return this->ship->getShipRoute().empty();};

    int getCurrentVisitNumber() { return get<0>(this->portCounter.find(this->getShip()->getCurrentPort())->second); }

    void errorsToFile(const string &fileName) const;

    const  string& getTravelPath(){ return  this->travelPath;}

    ~Travel() {
        delete this->ship;
    };

private:
    void increaseVisits(const string &port);

    tuple<int, int> getVisits(const string &port);

};

#endif //SHIPGIT_TRAVEL_H
