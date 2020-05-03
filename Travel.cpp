#include "Travel.h"
#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <math.h>

namespace fs = std::filesystem;
using std::cerr;
using std::make_tuple;
using std::ifstream;
using std::__cxx11::to_string;
using std::stringstream;
using std::ofstream;


Travel::Travel(const string &travelPath, const string &travelName, shared_ptr<ShipMap> shipMap, list<string> route,
               int routeError,
               int shipError) {
    this->travelPath = travelPath;
    this->travelName = travelName;
    this->shipMap = shipMap;
    this->route = route;

    for (const string &port : this->route) {
        auto it = portCounter.find(port);
        if (it == portCounter.end()) {
            portCounter.insert(make_pair(port, make_tuple(0, 1)));
        } else {
            get<1>(it->second) = get<1>(it->second) + 1;
        }
    }
    if (!this->route.empty()) {
        this->increaseVisits(this->route.front());
    }

    this->routeError = routeError;
    this->shipPlanError = shipError;
}

Travel::Travel(const Travel &other) {
    this->shipMap = std::make_shared<ShipMap>(*other.shipMap);
    this->travelPath = other.travelPath;
    this->portCounter = other.portCounter;
    this->travelName = other.travelName;
    this->route = other.route;
}

bool does_file_exist(const string &fileName) {
    ifstream inFile(fileName);
    if (inFile) {
        inFile.close();
        return true;
    } else {
        return false;
    }
}

tuple<int, int> Travel::getVisits(const string &port) {
    auto it = portCounter.find(port);
    return it->second;
}

void Travel::increaseVisits(const string &port) {
    auto it = portCounter.find(port);
    get<0>(it->second) = get<0>(it->second) + 1;
}

void Travel::goToNextPort() {
    if (!this->route.empty()) {
        this->route.pop_front();
        if (!this->route.empty()) {
            increaseVisits(this->route.front());
        }
    }
}

int Travel::getContainerList(const string &errorFile, list<shared_ptr<Container>> &contList) {
    list<shared_ptr<Container>> checkList = {};
    string currentPort = this->route.front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    string fileName = travelPath + "/" + currentPort + "_" + to_string(visitNum) + ".cargo_data";
    if (this->route.size() == 1) {
        FileHandler::fileToContainerList(fileName, checkList);
        if (!checkList.empty()) {
            return int(pow(2, 17));
        }
        return 0;
    }
    return FileHandler::fileToContainerList(fileName, contList, errorFile + "/" + travelName + "cargoDataErrors");
}

string Travel::getTravelName() const {
    return this->travelName;
}

//void Travel::setToOriginalTravel() {
//    this->getShip()->setShipRoute(originalRoute);
//    this->shipMap->clearContainers();
//
//    for (auto &pair : this->portCounter) {
//        get<0>(pair.second) = 0;
//    }
//
//    if (!this->ship->getShipRoute().empty()) {
//        this->increaseVisits(this->ship->getShipRoute().front());
//    }
//}

void Travel::errorsToFile(const string &fileName) const {
    ofstream outfile;
    outfile.open(fileName + "/" + this->getTravelName() + "FileErrors", std::ios::app);
    if (!outfile) {
        return;
    }

    unordered_map copyMap = this->portCounter;

    for (auto &pair : copyMap) {
        get<0>(pair.second) = 0;
    }


    for (std::list<std::string>::const_iterator i = this->route.begin();
         i != this->route.end(); ++i) {
        int visitNum = get<0>(copyMap.find(i->c_str())->second) += 1;

        string fileName = travelPath + "/" + i->c_str() + "_" + to_string(visitNum) + ".cargo_data";
        if (next(i) == this->route.end()) {
            if (does_file_exist(
                    travelPath + "/" + i->c_str() + "_" + to_string(visitNum) + ".cargo_data")) {
                outfile << "Warning, final port: " << i->c_str() << " in " << travelPath
                        << " should not have a cargo data file\n";
            }
        } else {
            if (!does_file_exist(fileName)) {
                outfile << "Warning, missing file: " << travelPath << "/" << i->c_str() << "_" << visitNum
                        << ".cargo_data\n";
            }
        }

    }


    size_t cargoFiles = 0;
    size_t otherFiles = 0;
    for (auto &path: fs::directory_iterator(travelPath)) {
        if (path.path().filename().extension().string().compare(".cargo_data") == 0) {
            cargoFiles++;
        } else {
            otherFiles++;
        }
    }


    if (cargoFiles >= this->route.size()) {
        outfile << "Warning, too many cargo_data files in travel folder\n";
    }
    if (otherFiles > 2) {
        outfile << "Warning, too many regular files in travel folder\n";
    }
    outfile.close();


}

const string Travel::getNextCargoFilePath() {
    string currentPort = this->route.front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    return travelPath + "/" + currentPort + "_" + to_string(visitNum) + ".cargo_data";
}
