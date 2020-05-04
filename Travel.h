/*Data structure to control flow of a specific travel of a ship via ports*/
#ifndef SHIPGIT_TRAVEL_H
#define SHIPGIT_TRAVEL_H

#include <unordered_map>
#include <iostream>
#include "Container.h"
#include "Ship.h"
#include "ShipMap.h"

using std::tuple;
using std::string;
using std::unordered_map;
using std::list;
using std::get;

class Travel {
    unordered_map<string, tuple<int, int>> portCounter;
    shared_ptr<ShipMap> shipMap;
    list<string> route;
    string travelPath, travelName, shipPath, routePath;
    int travelError = 0;
public:

    Travel(const string &travelPath, const string &travelName, const string&shipPath, const string& routePath, shared_ptr<ShipMap> ship,list<string> route, int travelError);

    Travel(const Travel &other);

    shared_ptr<ShipMap> getShipMap() { return this->shipMap; }

    list<string>& getRoute() { return this->route; }

    string getTravelName() const;

    int getContainerList(const string &errorFile, list<shared_ptr<Container>> &contList);

    void goToNextPort();

    int getTravelError() { return this->travelError; };

    bool isTravelLegal();

    bool didTravelEnd() { return this->route.empty(); };

    int getCurrentVisitNumber() { return get<0>(this->portCounter.find(this->route.front())->second); }

    void generalTravelErrorsToFile(const string &fileName) const;

    const string &getTravelPath() { return this->travelPath; }

    const string getNextCargoFilePath();

    string &getCurrentPort(){ return this->route.front();};
    const string &getShipPlanPath(){ return this->shipPath;};
    const string &getRoutePath(){ return this->routePath;};



private:
    void increaseVisits(const string &port);

    tuple<int, int> getVisits(const string &port);

    void initPortCounter();

};

#endif //SHIPGIT_TRAVEL_H
