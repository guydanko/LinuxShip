#include "Travel.h"
#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
using std::cerr;
using std::make_tuple;
using std::ifstream;
using std::__cxx11::to_string;
using std::stringstream;
using std::ofstream;

Travel::Travel(const string &travelPath, const string &travelName, Ship *ship) {
    this->travelPath = travelPath;
    this->travelName = travelName;
    this->ship = ship;

    for (const string &port : this->ship->getShipRoute()) {
        auto it = portCounter.find(port);
        if (it == portCounter.end()) {
            portCounter.insert(make_pair(port, make_tuple(0, 1)));
        } else {
            get<1>(it->second) = get<1>(it->second) + 1;
        }
    }
    if (!this->ship->getShipRoute().empty()) {
        this->increaseVisits(this->ship->getShipRoute().front());
    }
    this->originalRoute = this->getShip()->getShipRoute();
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
    this->getShip()->sailToNextPort();
    if (this->ship->getShipRoute().size() != 0) {
        increaseVisits(this->ship->getShipRoute().front());
    }
}

list<Container *> Travel::getContainerList(const string &errorFile) {
    if (this->ship->getShipRoute().empty()) {
        return list<Container *>();
    }
    if (this->getShip()->getShipRoute().size() == 1) {
        return list<Container *>();
    }
    string currentPort = this->getShip()->getShipRoute().front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    string fileName = travelPath + "/" + currentPort + "_" + to_string(visitNum) + ".cargo_data";
    if (!does_file_exist(fileName)) {
        return list<Container *>();
    } else {
        return FileHandler::fileToContainerList(fileName, errorFile + "/" + travelName + "cargoDataErrors");
    }
}

string Travel::getTravelName() const {
    return this->travelName;
}

void Travel::setToOriginalTravel() {
    this->getShip()->setShipRoute(originalRoute);
    this->ship->getShipMap().clearContainers();

    for (auto &pair : this->portCounter) {
        get<0>(pair.second) = 0;
    }

    if (!this->ship->getShipRoute().empty()) {
        this->increaseVisits(this->ship->getShipRoute().front());
    }
}

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


    for (std::list<std::string>::const_iterator i = this->originalRoute.begin(); i != this->originalRoute.end(); ++i) {
        int visitNum = get<0>(copyMap.find(i->c_str())->second) += 1;

        string fileName = travelPath + "/" + i->c_str() + "_" + to_string(visitNum) + ".cargo_data";
        if (next(i) == this->originalRoute.end()) {
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
        if(path.path().filename().extension().string().compare(".cargo_data") == 0) {
            cargoFiles++;
        } else{
            otherFiles++;
        }
    }


    if (cargoFiles >= this->originalRoute.size()) {
        outfile << "Warning, too many cargo_data files in travel folder\n";
    }
    if(otherFiles > 2){
        outfile << "Warning, too many regular files in travel folder\n";
    }
    outfile.close();


}

const string Travel::getNextCargoFilePath(){
    string currentPort = this->getShip()->getShipRoute().front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    return travelPath + "/" + currentPort + "_" + to_string(visitNum) + ".cargo_data";
}
