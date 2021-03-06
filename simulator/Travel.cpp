#include "Travel.h"

bool Travel::isTravelErrorLegal(int errorCode) {
    return !(errorCode & (1 << 3) || errorCode & (1 << 4) || errorCode & (1 << 7) ||
             errorCode & (1 << 8));
}

bool isValidPositiveNumber(const string &s) {
    if (s.empty()) {
        return false;
    }
    for (unsigned int i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    if (stoi(s) <= 0) { return false; }
    return true;
}

bool Travel::isFileInTravelRoute(const string &fileName) const {
    vector<string> tokens;
    stringstream sstream(fileName);
    string token;

    while (getline(sstream, token, '_')) {
        tokens.push_back(token);
    }

    //not in map or not in format
    if (tokens.size() != 2 || this->portCounter.find(tokens[0]) == this->portCounter.end()) {
        return false;
    }

    if (!isValidPositiveNumber(tokens[1])) { return false; }

    //larger than max visit
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

int Travel::getContainerList(ostream &outFile, list<shared_ptr<Container>> &contList) {
    list<shared_ptr<Container>> checkList = {};
    string currentPort = this->route.front();

    tuple<int, int> visits = getVisits(currentPort);
    int visitNum = get<0>(visits);

    string fileName = travelPath + "/" + currentPort + "_" + std::to_string(visitNum) + ".cargo_data";
    if (this->route.size() == 1) {
        int result = FileHandler::fileToContainerList(fileName, checkList);
        /*no file for last port - OK*/
        if (result & (1 << 16)) {
            return 0;
        }
            /*there were containers in last port file, or there was corrupt line*/
        else if (!checkList.empty() || result > 0) { return 1 << 17; }
        /*file exists but not lines in it*/
        return 0;

    }
    return FileHandler::fileToContainerList(fileName, contList, outFile,
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
    std::error_code er;
    list<string> missingCargoFiles = {};
    unordered_map copyMap = this->portCounter;

    for (auto &pair : copyMap) {
        get<0>(pair.second) = 0;
    }

    for (std::list<std::string>::const_iterator i = this->route.begin(); i != this->route.end(); ++i) {
        if (next(i) == this->route.end()) {
            continue;
        }
        int visitNum = get<0>(copyMap.find(i->c_str())->second) += 1;
        string fileName = travelPath + "/" + i->c_str() + "_" + std::to_string(visitNum) + ".cargo_data";
        if (!fs::exists(fileName, er)) {
            missingCargoFiles.emplace_back(std::string(i->c_str()) + "_" + std::to_string(visitNum) + ".cargo_data");
        }
    }

    return missingCargoFiles;
}

list<string> Travel::getUnusedFiles() const {
    list<string> unusedFiles = {};
    std::error_code er;
    for (auto &path:fs::directory_iterator(travelPath, er)) {
        const string extension = path.path().filename().extension().string();
        if (extension == ".cargo_data") {
            if (!isFileInTravelRoute(path.path().stem().string())) {
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

    return travelPath + "/" + currentPort + "_" + std::to_string(visitNum) + ".cargo_data";
}

void Travel::operator=(Travel &&other) {

    if (this != &other) {
        this->shipMap = std::move(other.shipMap);
        this->travelPath = other.travelPath;
        this->portCounter = other.portCounter;
        this->travelName = other.travelName;
        this->route = other.route;
        this->travelError = other.travelError;
        this->shipPath = other.shipPath;
        this->routePath = other.routePath;
        this->travelError = other.travelError;
    }
}
