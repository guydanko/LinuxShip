#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "SimulatorError.h"
#include <unordered_set>
#include <unordered_map>

using std::ifstream;
using std::cerr;
using std::stringstream;
using std::ofstream;
using std::unordered_set;
using std::unordered_map;

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

bool isNumber(const string &s) {
    for (unsigned int i = 0; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

void strToUpper(string &str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        str[i] = toupper(str[i]);
    }
}

list<shared_ptr<Container>> FileHandler::fileToContainerList(const string &fileName, const string &errorFile) {
    list<shared_ptr<Container>>
            containers = {};
    ifstream inFile;
    inFile.open(fileName);
    ofstream outFile(errorFile, std::ios::app);
    int lineNum = 0;

    vector<string> tokens;
    /*could not open file*/
    if (!inFile) {
        outFile << "Could not open file: " << fileName << "\n";
        outFile.close();
        return containers;
    }

    string line;
    unordered_set<string> id_map;

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            svec.push_back(trim(token));
        }

        if (svec.size() != 3) {
            outFile << "Warning, file: " << fileName << " line number: " << lineNum << " is not in valid format!\n";
            continue;
        }

        string id = svec[0], weight = svec[1], destination = svec[2];

        if (isNumber(weight) && Container::isLegalParamContainer(stoi(weight), destination, id)) {
            strToUpper(destination);
            strToUpper(id);
            if (id_map.find(id) == id_map.end()) {
                id_map.insert(id);
                containers.emplace_back(std::make_shared<Container>(stoi(weight), destination, id));
            } else {
                outFile << "Warning, file: " << fileName << " line number: " << lineNum
                        << " is a container with duplicate ID!\n";
            }

        } else {
            outFile << "Warning, file: " << fileName << " line number: " << lineNum << " is not a valid container!\n";
            containers.emplace_back(std::make_shared<Container>(0, "", id, false));
        }


    }

    inFile.close();
    outFile.close();
    return containers;

}

list<string> FileHandler::fileToRouteList(const string &fileName, const string &errorFile) {
    list<string> routes = {};
    ifstream inFile;
    inFile.open(fileName);
    ofstream outFile(errorFile, std::ios::app);
    int lineNum = 0;

    /*could not open file*/
    if (!inFile) {
        outFile << "Could not open file: " << fileName << "\n";
        outFile.close();
        return routes;
    }

    string line;
    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, '\n')) {
            svec.push_back(trim(token));
        }

        if (svec.size() != 1) {
            outFile << "Warning: file" << fileName << "line number: " << lineNum << " is not in valid format!\n";
            continue;
        }

        string port = svec[0];
        if (Container::isPortValid(port)) {
            strToUpper(port);
            if (routes.size() > 0 && routes.back().compare(port) == 0) {
                outFile << "Warning, file: " << fileName << "line number: " << lineNum
                        << " repeats the same twice in a row!\n";
            } else {
                routes.push_back(port);
            }
        } else {
            outFile << "Warning, file: " << fileName << " line number: " << lineNum << " is not a legal port!\n";
        }

    }
    if (routes.empty()) {
        outFile << "Could not create route from file: " << fileName << ", file does not contain any legal ports\n";
    }

    inFile.close();
    outFile.close();
    return routes;

}

void FileHandler::operationsToFile(list<CargoOperation> operations, const string &fileName,
                                   const string &currentPort) {
    ofstream outfile;
    outfile.open(fileName, std::ios::app);
    if (!outfile) {
        return;
    }

    for (CargoOperation &op: operations) {
        outfile << op << "\n";
    }

    outfile.close();
}

Ship *FileHandler::createShipFromFile(const string &fileName, const string &errorFileName) {
    ifstream inFile(fileName);
    ofstream errorFile(errorFileName, std::ios::app);
    /*could not open file*/
    if (!inFile) {
        errorFile << "Could not create ship from file : " << fileName << "\n";
        errorFile.close();
        return nullptr;
    }

    string line, token;
    vector<string> svec;
    int lineNum = 1;

    while (getline(inFile, line)) {
        if (line[0] != '#') {
            break;
        }
    }
    stringstream sline(line);
    while (getline(sline, token, ',')) {
        token = trim(token);
        if (!isNumber(token)) {
            errorFile << "Error: details of ship must be numbers!\n";
            errorFile.close();
            return nullptr;
        }
        svec.push_back(token);
    }

    int height = stoi(svec[0]), rows = stoi(svec[1]), cols = stoi(svec[2]);
    Ship *ship = new Ship(height, rows, cols);
    vector<vector<int>> indexVector(rows, vector<int>(cols, 0));

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            token = trim(token);
            if (!isNumber(token)) {
                break;
            }
            svec.push_back(token);
        }

        if (svec.size() != 3) {
            errorFile << "Warning, file: " << fileName << " line number: " << lineNum << " is not in valid format!\n";
            continue;
        }
        int actualFloors = stoi(svec[2]), row = stoi(svec[0]), col = stoi(svec[1]);
        if (actualFloors >= ship->getShipMap().getHeight()) {
            errorFile << "Warning, file: " << fileName << " actual floors in line: " << lineNum
                      << " is larger or equal to max height\n";
            continue;
        }
        if (row >= ship->getShipMap().getRows() || col >= ship->getShipMap().getCols()) {
            errorFile << "Warning, file: " << fileName << " dimensions in line: " << lineNum
                      << " are larger than dimension of floor\n";
            continue;
        }
        if (indexVector[row][col] != 0) {
            errorFile << "Warning, file: " << fileName << " line: " << lineNum
                      << " used indexes that were already assigned\n";
        } else {
            indexVector[row][col] = 1;
            ship->getShipMap().initShipMapContainer(ship->getShipMap().getHeight() - actualFloors, row, col);
        }
    }
    inFile.close();
    errorFile.close();
    return ship;
}

void
FileHandler::simulatorErrorsToFile(const list<SimulatorError> &simErrors, const string &path, const string &travelName,
                                   const string &portName, int visitNumber, bool noErrors,
                                   const string &errorFileName) {
    ofstream outFile;
    outFile.open(path + "/" + travelName + "AlgoErrors", std::ios::app);
    ofstream errorFile(errorFileName, std::ios::app);
    if (!outFile) {
        errorFile << "Could not write error file: " << path + "/" + travelName + "AlgoErrors" << "\n";
        errorFile.close();
        return;
    }
    if (noErrors) {
        outFile << "No Errors, Algorithm is correct!" << "\n";
        outFile.close();
        errorFile.close();
        return;
    }

    if (!simErrors.empty() && simErrors.front().getErrorType() != SimErrorType::TRAVEL_END) {
        outFile << "Simulation Errors in port: " << portName << " ,visit no: " << visitNumber << "\n";
    }

    for (const SimulatorError &simError:simErrors) {
        outFile << simError << "\n";
    }

    errorFile.close();
    outFile.close();
}

list<shared_ptr<CargoOperation>>
FileHandler::createCargoOpsFromFile(const string &fileName, list<shared_ptr<Container>> &containerList,unordered_map<string, list<shared_ptr<Container>>>& containerMap) {
    list<shared_ptr<CargoOperation>> ops = {};
    ifstream inFile(fileName);

    /*could not open file*/
    if (!inFile) {
        return ops;
    }

    string line;

    while (getline(inFile, line)) {
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            token = trim(token);
            svec.push_back(token);
        }

        AbstractAlgorithm::Action action;
        shared_ptr<Container> cont= nullptr;
        if(!containerMap[svec[1]].empty() ){
            if(action==AbstractAlgorithm::Action::REJECT){
                cont = containerMap[svec[1]].front();
                containerMap[svec[1]].pop_front();
            }
            else{
                std::cout<<"im here 2 "<<svec[1]<<std::endl;
                cont = containerMap[svec[1]].front();
            }
        }
        else{
            std::cout<<"im here "<<svec[1]<<std::endl;
        }



        switch (svec[0].c_str()[0]) {
            case 'L':
                action = AbstractAlgorithm::Action::LOAD;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]), stoi(svec[4]))));
                break;
            case 'M':
                action = AbstractAlgorithm::Action::MOVE;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]), int(svec[4].c_str()[0]) - 48),
                                 MapIndex(stoi(svec[5]), stoi(svec[6]), int(svec[7].c_str()[0]) - 48)));
                break;
            case 'R':
                action = AbstractAlgorithm::Action::REJECT;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont));
                break;
            case 'U':
                action = AbstractAlgorithm::Action::UNLOAD;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]), stoi(svec[4]))));
                break;
        }


    }

    inFile.close();
    return ops;
}