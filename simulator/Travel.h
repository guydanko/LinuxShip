/*Data structure to control flow of a specific travel of a ship via ports*/
#ifndef SHIPGIT_TRAVEL_H
#define SHIPGIT_TRAVEL_H

#include <unordered_map>
#include <iostream>
#include "../common/Container.h"
#include "../common/ShipMap.h"

using std::tuple;
using std::string;
using std::unordered_map;
using std::list;
using std::get;

class Travel {
    unordered_map<string, tuple<int, int>> portCounter;
    std::shared_ptr<ShipMap> shipMap;
    list<string> route;
    string travelPath, travelName, shipPath, routePath;
    int travelError = 0;
public:

    Travel(const string &travelPath, const string &travelName, const string&shipPath, const string& routePath, shared_ptr<ShipMap> ship,list<string> route, int travelError);

    Travel(const Travel &other);

    ShipMap* getShipMap() { return this->shipMap.get(); }

    list<string>& getRoute() { return this->route; }

    string getTravelName() const;

    int getContainerList(const string &errorFile, list<shared_ptr<Container>> &contList);

    void goToNextPort();

    int getTravelError() { return this->travelError; };

    bool isTravelLegal();

    bool didTravelEnd() { return this->route.empty(); };

    int getCurrentVisitNumber() { return get<0>(this->portCounter.find(this->route.front())->second); }

    void generalTravelErrorsToFile(const string &fileName) const;

    const string getNextCargoFilePath();

    string &getCurrentPort(){ return this->route.front();};
    const string &getShipPlanPath(){ return this->shipPath;};
    const string &getRoutePath(){ return this->routePath;};

    static bool isTravelErrorLegal(int errorCode);



private:
    void increaseVisits(const string &port);

    tuple<int, int> getVisits(const string &port);

    void initPortCounter();

    list<string> getMissingCargoFiles() const;

    list<string> getUnusedFiles() const;

    bool isFileInTravelRoute(const string& fileName)const;

};

#endif //SHIPGIT_TRAVEL_H
