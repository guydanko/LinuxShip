#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "SimulatorError.h"

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

list<Container *> FileHandler::fileToContainerList(const string &fileName) {
    list<Container *> containers = {};
    ifstream inFile;
    inFile.open(fileName);
    int lineNum = 0;

    vector<string> tokens;

    /*could not open file*/
    if (!inFile) {
        cerr << "Could not open file\n";
        return containers;
    }

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

        if (svec.size() != 3) {
            cerr << "Warning, file: " << fileName << " line number: " << lineNum << " is not in valid format!\n";
            continue;
        }

        string id, weight, destination;

        id = svec[0];
        weight = svec[1];
        destination = svec[2];

        Container *cont;

        if (isNumber(weight) && Container::isLegalParamContainer(stoi(weight), destination, id)) {
            strToUpper(destination);
            strToUpper(id);

            cont = new Container(stoi(weight), destination, id);
        } else {
            cont = new Container(0, "", id, false);
        }
        containers.push_back(cont);

    }

    inFile.close();
    return containers;

}

list<string> FileHandler::fileToRouteList(const string &fileName) {
    list<string> routes = {};
    ifstream inFile;
    inFile.open(fileName);
    int lineNum = 0;

    /*could not open file*/
    if (!inFile) {
        cerr << "Could not open file\n";
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
            cerr << "Warning: file" << fileName << "line number: " << lineNum << " is not in valid format!\n";
            continue;
        }

        string port = svec[0];

        if (Container::isPortValid(port)) {
            strToUpper(port);
            if (routes.size() > 0 && routes.back().compare(port) == 0) {
                cerr << "Warning, file: " << fileName << "line number: " << lineNum
                     << " repeats the same twice in a row!\n";
            } else {
                routes.push_back(port);
            }
        } else {
            cerr << "Warning, file: " << fileName << " line number: " << lineNum << " is not a legal port!\n";
        }

    }

    inFile.close();
    return routes;

}

void FileHandler::operationsToFile(list<CargoOperation> operations, const string &fileName, const string &travelName,
                                   const string &currentPort, int visitNumber) {
    ofstream outfile;
    outfile.open(fileName + "/" + travelName + "AllOperations.txt", std::ios::app);
    if (!outfile) {
        cerr << "Could not open file: " << fileName << "for writing\n";
        return;
    }


    if (operations.size() > 0) {
        outfile << "All operations in port: " << currentPort << " ,visit no:" << visitNumber << "\n";
        for (CargoOperation &op: operations) {
            outfile << op << "\n";
        }

    } else {
        outfile << "No operations performed in port: " << currentPort << " ,visit no:" << visitNumber << "\n";
    }


    outfile.close();

}

Ship *FileHandler::createShipFromFile(const string &fileName) {
    ifstream inFile(fileName);
    /*could not open file*/
    if (!inFile) {
        std::cerr << "Could not open file\n";
        return nullptr;
    }

    string line;
    vector<string> svec;
    string token;
    int lineNum = 1;

    getline(inFile, line);
    stringstream sline(line);

    while (getline(sline, token, ',')) {
        token = trim(token);
        if (!isNumber(token)) {
            cerr << "Error: details of ship must be numbers!\n";
            return nullptr;
        }
        svec.push_back(token);
    }

    int height = stoi(svec[0]), rows = stoi(svec[1]), cols = stoi(svec[2]);
    Ship *ship = new Ship(height, rows, cols);

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#') { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            token = trim(token);

            if (!isNumber(token)) {
                cerr << "Warning, file:" << fileName << " line number:" << lineNum
                     << " details of ship must be positive numbers!\n";
                break;
            }
            svec.push_back(token);
        }

        if (svec.size() != 3) {
            cerr << "Warning, file: " << fileName << " line number: " << lineNum << "is not in valid format!\n";
            continue;
        }

        int actualFloors = stoi(svec[0]), row = stoi(svec[1]), col = stoi(svec[2]);

        if (actualFloors >= ship->getShipMap().getHeight()) {
            cerr << "Warning, file: " << fileName << " actual floors in line: " << lineNum
                 << " is larger or equal to max height\n";
            continue;
        }

        if (row >= ship->getShipMap().getRows() || col >= ship->getShipMap().getCols()) {
            cerr << "Warning, file: " << fileName << " dimensions in line: " << lineNum
                 << " are larger than dimension of floor\n";
            continue;
        }

        ship->getShipMap().initShipMapContainer(ship->getShipMap().getHeight() - actualFloors, row, col);
    }

    inFile.close();
    return ship;
}

void
FileHandler::simulatorErrorsToFile(const list<SimulatorError> &simErrors, const string &path, const string &travelName,
                                   const string &portName, int visitNumber, bool noErrors) {
    ofstream outFile;
    outFile.open(path + "/" + travelName + "AlgoErrors.txt", std::ios::app);
    if (!outFile) {
        cerr << "Could not write error file: " << path << "\n";
        return;
    }

    if (noErrors) {
        outFile << "No Errors, Algorithm is correct!" << "\n";
        outFile.close();
        return;
    }

    if (simErrors.size() > 0) {
        outFile << "Errors in port: " << portName << " ,visit no: " << visitNumber << "\n";
        for (const SimulatorError &simError:simErrors) {
            outFile << simError << "\n";
        }
    }

    outFile.close();
}
