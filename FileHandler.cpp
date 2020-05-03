#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "SimulatorError.h"
#include <unordered_map>


using std::ifstream;
using std::cerr;
using std::stringstream;
using std::ofstream;


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

void strToUpper(string &str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        str[i] = toupper(str[i]);
    }
}

bool needToWrite(const string &fileName) {
    return !fileName.empty();
}

int FileHandler::fileToContainerList(const string &fileName, list<shared_ptr<Container>> &containerList,
                                     const string &errorFile) {
    int result = 0;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile);

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile << "File: " << fileName << " does not exist\n";
            outFile.close();
        }
        return (1 << 16);
    }

    int lineNum = 0;
    vector<string> tokens;
    string line;

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            svec.push_back(trim(token));
        }

        if (svec.size() > 3) {
            if (toWrite) {
                outFile << "Warning, file: " << fileName << " line number: " << lineNum << " is not in valid format!\n";
            }
            containerList.emplace_back(std::make_shared<Container>(0, "", svec[1], false));
            continue;
            //to fix later depending on what error this is
        }

        size_t vecSize = svec.size();
        string id, weight, destination;
        id = vecSize >= 1 ? svec[0] : "";
        weight = vecSize >= 2 ? svec[1] : "";
        destination = vecSize >= 3 ? svec[2] : "";
        strToUpper(destination);
        strToUpper(id);

        string errorMessage = "error code: ";

        if (weight.empty() || !isNumber(weight)) {
            errorMessage += "2^12  ";
            result |= (1 << 12);
        }
        if (destination.empty() || !Container::isPortValid(destination)) {
            errorMessage += "2^13  ";
            result |= (1 << 13);
        }
        if (id.empty()) {
            errorMessage += "2^14  ";
            return result |= (1 << 14);
        } else {
            if (!Container::isLegalId(id)) {
                errorMessage += "2^15  ";
                return result |= (1 << 15);
            }
        }

        if (isNumber(weight) && Container::isLegalParamContainer(stoi(weight), destination, id)) {
            containerList.emplace_back(std::make_shared<Container>(stoi(weight), destination, id));
        } else {
            if (toWrite) {
                outFile << " line number: " << lineNum
                        << " is not a valid container! ( " << errorMessage << " )\n";
            }
            containerList.emplace_back(std::make_shared<Container>(0, "", id, false));
        }


    }

    inFile.close();
    outFile.close();
    return result;

}

int FileHandler::fileToRouteList(const string &fileName, list<string> &route, const string &errorFile) {
    int result = 0;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile);

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile << "Could not open file: error 2^7" << fileName << "\n";
            outFile.close();
        }
        return (1 << 7);
    }

    int lineNum = 0;
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
            if (toWrite) {
                outFile << "line number: " << lineNum << " is not in valid format!\n";
            }
            //something with result?
            continue;
        }

        string port = svec[0];
        if (Container::isPortValid(port)) {
            strToUpper(port);
            if (route.size() > 0 && route.back().compare(port) == 0) {
                if (toWrite) {
                    outFile << "in line: " << lineNum
                            << " 2^5 - travel route: a port appears twice or more consecutively (ignored)\n";
                }
                result |= (1 << 5);
            } else {
                route.push_back(port);
            }
        } else {
            if (toWrite) {
                outFile  << "in line: " << lineNum << " 2^6 - travel route: bad port symbol format (ignored)\n";
            }
            result |= (1 << 6);
        }

    }
    if (route.empty()) {
        if (toWrite) {
            outFile << "Could not create route from file: " << fileName << ", 2^7 - travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
        }
        result |= (1 << 7);
    }

    if (route.size() == 1) {
        if (toWrite) {
            outFile << "Could not create route from file: " << fileName << ", 2^8 - travel route: travel error - file with only a single valid port (cannot run this travel)\n";
        }
        result |= (1 << 8);
    }

    inFile.close();
    outFile.close();

    return result;

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

int FileHandler::createShipMapFromFile(const string &fileName, shared_ptr<shared_ptr<ShipMap>> shipPtr,
                                       const string &errorFile) {
    int result = 0;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile);

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile << "Could not create ship from file : " << fileName << " 2^3 - ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n" ;
            outFile.close();
        }
        return result | (1 << 3);
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
            if (toWrite) {
                outFile << "Error: first line of shipPlan in file: " << fileName << " 2^3 - ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
                outFile.close();
            }
            return result | (1 << 3);
        }
        svec.push_back(token);
    }

    int height = stoi(svec[0]), rows = stoi(svec[1]), cols = stoi(svec[2]);
    *shipPtr = std::make_shared<ShipMap>(height, rows, cols);
    vector<vector<int>>
            indexVector(rows, vector<int>(cols, 0));

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
            if (toWrite) {
                outFile << "in line: " << lineNum
                        << " 2^2 - ship plan: bad line format after first line or duplicate x,y appearance with same data (ignored)\n";
            }
            result |= (1 << 2);
            continue;
        }
        int actualFloors = stoi(svec[2]), row = stoi(svec[0]), col = stoi(svec[1]);
        if (actualFloors >= height) {
            if (toWrite) {
                outFile << "in line: " << lineNum
                        << " 2^0 - ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n";
            }
            result |= 1;
            continue;
        }
        if (row >= rows ||
            col >= cols) {
            if (toWrite) {
                outFile  << "in line: " << lineNum
                        << " 2^1 - ship plan: a given position exceeds the X/Y ship limits (ignored)\n";
            }
            result |= (1 << 1);
            continue;
        }
        if (indexVector[row][col] != 0) {
            if (toWrite) {
                outFile << "in line: " << lineNum
                        << " used indexes that were already assigned\n";
            }
            //something with result error?
        } else {
            indexVector[row][col] = 1;
            (*(*shipPtr)).initShipMapContainer(height - actualFloors, row, col);
        }
    }
    inFile.close();
    outFile.close();
    return result;
}

void
FileHandler::simulatorErrorsToFile(const list<SimulatorError> &simErrors, const string &path, const string &travelName,
                                   const string &portName, int visitNumber,
                                   const string &errorFileName) {
    ofstream outFile;
    outFile.open(path + ".errors", std::ios::app);
    ofstream errorFile(errorFileName, std::ios::app);
    if (!outFile) {
        errorFile << "Could not write error file: " << path + "/" + travelName + "AlgoErrors" << "\n";
        errorFile.close();
        return;
    }
//    if (noErrors) {
//        outFile << "No Errors, Algorithm is correct!" << "\n";
//        outFile.close();
//        errorFile.close();
//        return;
//    }

    if (!simErrors.empty() && simErrors.front().getErrorType() != SimErrorType::TRAVEL_END && simErrors.front().getErrorType() != SimErrorType::TRAVEL_INIT) {
        outFile<<"\n";
        outFile << "Simulation Errors in port: " << portName << " ,visit no: " << visitNumber << "\n";
    }

    for (const SimulatorError &simError:simErrors) {
        outFile << simError << "\n";
    }
    errorFile.close();
    outFile.close();
}

list<shared_ptr<CargoOperation>>
FileHandler::createCargoOpsFromFile(const string &fileName, list<shared_ptr<Container>> &containerList) {
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
        shared_ptr<Container> cont =
                svec.size() > 1 ? std::make_shared<Container>(0, "", svec[1]) : std::make_shared<Container>(0, "", "");

        switch (svec[0].c_str()[0]) {
            case 'L':
                action = AbstractAlgorithm::Action::LOAD;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]),
                                                                                         stoi(svec[4]))));
                break;
            case 'M':
                action = AbstractAlgorithm::Action::MOVE;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]),
                                                                                         int(svec[4].c_str()[0]) - 48),
                                                                  MapIndex(stoi(svec[5]), stoi(svec[6]),
                                                                           int(svec[7].c_str()[0]) - 48)));
                break;
            case 'R':
                action = AbstractAlgorithm::Action::REJECT;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont));
                break;
            case 'U':
                action = AbstractAlgorithm::Action::UNLOAD;
                ops.emplace_back(std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]),
                                                                                         stoi(svec[4]))));
                break;
        }


    }

    inFile.close();
    return ops;
}