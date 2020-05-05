#include "Travel.h"
#include "FileHandler.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;
using std::cerr;
using std::make_tuple;
using std::ifstream;
using std::__cxx11::to_string;
using std::stringstream;
using std::ofstream;

bool isValidNumber(const string &s) {
    if (s.empty()) {
        return false;
    }
    for (unsigned int i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

bool Travel::isFileInTravelRoute(const string &fileName) const {
    vector<string> tokens;
    stringstream sstream(fileName);
    string token;

    while (getline(sstream, token, '_')) {
        tokens.push_back(token);
    }

    if (tokens.size() != 2 || this->portCounter.find(tokens[0]) == this->portCounter.end()) {
        return false;
    }

    if (!isValidNumber(tokens[1])) { return false; }

    if (std::stoi(tokens[1]) > get<1>(this->portCounter.find(tokens[0])->second)) { return false; }

    return true;
}

void Travel::initPortCounter() {
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
}

Travel::Travel(const string &travelPath, const string &travelName, const string &shipPath, const string &routePath,
               shared_ptr<ShipMap> shipMap, list<string> route,
               int travelError) {
    this->travelPath = travelPath;
    this->travelName = travelName;
    this->shipMap = shipMap;
    this->route = route;
    this->shipPath = shipPath;
    this->routePath = routePath;
    this->travelError = travelError;
    initPortCounter();
}

Travel::Travel(const Travel &other) {
    this->shipMap = std::make_shared<ShipMap>(*other.shipMap);
    this->travelPath = other.travelPath;
    this->portCounter = other.portCounter;
    this->travelName = other.travelName;
    this->route = other.route;
    this->travelError = other.travelError;
    this->shipPath = other.shipPath;
    this->routePath = other.routePath;
    this->travelError = other.travelError;
}

//bool does_file_exist(const string &fileName) {
//    ifstream inFile(fileName);
//    if (inFile) {
//        inFile.close();
//        return true;
//    } else {
//        return false;
//    }
//}

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
        if (fs::exists(fileName) && fs::is_empty(fileName)) { return 0; }
        return (1 << 17);
    }
    return FileHandler::fileToContainerList(fileName, contList, errorFile,
                                            this->route.front() + "_" + std::to_string(visitNum));
}

string Travel::getTravelName() const {
    return this->travelName;
}

bool Travel::isTravelLegal() {
    return !(this->travelError & (1 << 3) || this->travelError & (1 << 4) || this->travelError & (1 << 7) ||
             this->travelError & (1 << 8));
}

list<string> Travel::getMissingCargoFiles() const {
    list<string> missingCargoFiles = {};
    unordered_map copyMap = this->portCounter;

    for (auto &pair : copyMap) {
        get<0>(pair.second) = 0;
    }

    for (std::list<std::string>::const_iterator i = this->route.begin();
         i != this->route.end(); ++i) {
        int visitNum = get<0>(copyMap.find(i->c_str())->second) += 1;

        string fileName = travelPath + "/" + i->c_str() + "_" + to_string(visitNum) + ".cargo_data";
        if (!fs::exists(fileName)) {
            missingCargoFiles.emplace_back(std::string(i->c_str()) + "_" + std::to_string(visitNum) + ".cargo_data");
        }
    }

    return missingCargoFiles;
}

list<string> Travel::getUnusedFiles() const {
    list<string> unusedFiles = {};

    for (auto &path:fs::directory_iterator(travelPath)) {
        const string extension = path.path().filename().extension().string();
        if (extension.compare(".cargo_data") == 0) {
            if (!isFileInTravelRoute(path.path().stem().string())) {
                unusedFiles.emplace_back(path.path().filename().string());
            }
        } else {
            if (extension.compare(".route") != 0 && extension.compare(".ship_plan") != 0) {
                unusedFiles.emplace_back(path.path().filename().string());
            }
        }


    }
    return unusedFiles;
}

void Travel::generalTravelErrorsToFile(const string &fileName) const {
    ofstream outfile;
    outfile.open(fileName, std::ios::app);
    if (!outfile) {
        return;
    }

    /*copy the current port map*/
    for (string &missingFile:this->getMissingCargoFiles()) {
        outfile << "missing file: " << missingFile << "\n";
    }
    for (string &unusedFile:this->getUnusedFiles()) {
        outfile << "unused file: " << unusedFile << "\n";
    }

    outfile.close();

}


const string Travel::getNextCargoFilePath() {
    string currentPort = this->route.front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    return travelPath + "/" + currentPort + "_" + to_string(visitNum) + ".cargo_data";
}
