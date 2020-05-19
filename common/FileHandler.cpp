#include "../common/FileHandler.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <filesystem>


using std::ifstream;
using std::cerr;
using std::stringstream;
using std::ofstream;
namespace fs = std::filesystem;

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

bool isPositiveNumber(const string &s) {
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

bool isLineEmpty(const std::string &s) {
    for (size_t i = 0; i < s.length(); i++) {
        if (!isspace(s[i])) {
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

bool areNumbers(const std::string &s1, const std::string &s2, const std::string &s3) {
    return isNumber(s1) && isNumber(s2) && isNumber(s3);
}

int FileHandler::fileToContainerList(const string &fileName, list<shared_ptr<Container>> &containerList,
                                     const string &errorFile, const string &portName) {
    int result = 0;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile) && outFile;
    string port = portName.empty() ? "" : portName + ": ";

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile << port <<
                    "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
            outFile.close();
        }
        return (1 << 16);
    }

    int lineNum = 0;
    vector<string> tokens;
    string line;

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#' || isLineEmpty(line)) {
            continue;
        }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            svec.push_back(trim(token));
        }

        if (svec.size() > 3) {
            if (toWrite) {
                outFile << port << "containers at port: bad line (" << lineNum << ") format\n";
            }
            result |= (1 << 14);
            continue;

        }

        size_t vecSize = svec.size();
        string id, weight, destination;
        id = vecSize >= 1 ? svec[0] : "";
        weight = vecSize >= 2 ? svec[1] : "";
        destination = vecSize >= 3 ? svec[2] : "";
        strToUpper(destination);
        strToUpper(id);

        string errorMessage = " ";

        if (weight.empty() || !isPositiveNumber(weight)) {
            errorMessage += "- missing or bad weight ";
            result |= (1 << 12);
        }
        if (destination.empty() || !Container::isPortValid(destination) || destination == portName) {
            errorMessage += "- missing or bad port dest ";
            result |= (1 << 13);
        }
        if (id.empty()) {
            errorMessage += "- ID cannot be read";
            result |= (1 << 14);
        } else {
            if (!Container::isLegalId(id)) {
                errorMessage += "- illegal ID check ISO 6346 ";
                result |= (1 << 15);
            }
        }
        if (isNumber(weight) && Container::isLegalParamContainer(stoi(weight), destination, id)) {
            containerList.emplace_back(std::make_shared<Container>(stoi(weight), destination, id));

            if (destination == portName && toWrite) {
                outFile << port << "containers at port: bad line (" << lineNum << ") format" << errorMessage
                        << "\n";
            }
        } else {
            if (toWrite) {
                outFile << port << "containers at port: bad line (" << lineNum << ") format" << errorMessage
                        << "\n";
            }
            if (!id.empty()) { containerList.emplace_back(std::make_shared<Container>(0, "", id, false)); }
        }
    }
    inFile.close();
    if (toWrite) { outFile.close(); }
    return result;
}

int FileHandler::fileToRouteList(const string &fileName, list<string> &route, const string &errorFile) {
    int result = 0;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile) && outFile;

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile
                    << "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
            outFile.close();
        }
        return (1 << 7);
    }

    int lineNum = 0;
    string line;
    while (getline(inFile, line)) {
        lineNum++;
        //skip line if it is whitespace or if its a note
        if (line[0] == '#' || isLineEmpty(line)) { continue; }

        stringstream sline(line);
        vector<string> svec;
        string token;

        //get route value
        while (getline(sline, token, '\n')) {
            svec.push_back(trim(token));
        }

        //more or less than one symbol
        if (svec.size() != 1) {
            result |= (1 << 6);
            if (toWrite) {
                outFile << "travel route: bad line (" << lineNum << ") " << "format (ignored)\n";
            }
            continue;
        }

        string port = svec[0];
        if (Container::isPortValid(port)) {
            strToUpper(port);
            if (route.size() > 0 && route.back() == port) {
                result |= (1 << 5);
                if (toWrite) {
                    outFile << "travel route: a port appears twice (line: " << lineNum
                            << ") or more consecutively (ignored)\n";
                }

            } else {
                route.push_back(port);
            }
        } else {
            result |= (1 << 6);
            if (toWrite) {
                outFile << "travel route: bad port symbol (line: " << lineNum
                        << " - ignored)\n";
            }
        }

    }


    if (route.empty()) {
        result |= (1 << 7);
        if (toWrite) {
            outFile
                    << "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
        }
    }

    if (route.size() == 1) {
        result |= (1 << 8);
        if (toWrite) {
            outFile
                    << "travel route: travel error - file with only a single valid port (cannot run this travel)\n";
        }
    }

    inFile.close();
    if (toWrite) { outFile.close(); }

    return result;

}

void FileHandler::operationsToFile(list<CargoOperation> &operations, const string &fileName) {
    ofstream outfile;
    outfile.open(fileName);
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
    bool toWrite = needToWrite(errorFile) && outFile;

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile
                    << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
            outFile.close();
        }
        return result | (1 << 3);
    }

    string line, token;
    vector<string> svec;
    int lineNum = 1;
    bool isRealFirstLine = false;

    while (getline(inFile, line)) {
        if (line[0] != '#' && !isLineEmpty(line)) {
            isRealFirstLine = true;
            break;
        }
        lineNum++;
    }

    if (!isRealFirstLine) {
        if (toWrite) {
            outFile
                    << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
            outFile.close();
        }
        return (1 << 3);
    }

    stringstream sline(line);
    while (getline(sline, token, ',')) {
        token = trim(token);
        if (!isNumber(token)) {
            if (toWrite) {
                outFile
                        << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
                outFile.close();
            }
            return (1 << 3);
        }
        svec.push_back(token);
    }


    int height = stoi(svec[0]), rows = stoi(svec[1]), cols = stoi(svec[2]);
    *shipPtr = std::make_shared<ShipMap>(height, rows, cols);
    vector<vector<int>>
            indexVector(rows, vector<int>(cols, -1));

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#' || isLineEmpty(line)) { continue; }
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
            result |= (1 << 2);
            if (toWrite) {
                outFile << "ship plan: bad line (" << lineNum << ") format after first line\n";
            }
            continue;
        }


        int actualFloors = stoi(svec[2]), row = stoi(svec[0]), col = stoi(svec[1]);
        if (actualFloors >= height) {
            if (toWrite) {
                outFile << "ship plan: a position (in line: " << lineNum
                        << ") has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n";
            }
            result |= 1;
            continue;
        }
        if (row >= rows ||
            col >= cols) {
            if (toWrite) {
                outFile << "ship plan: a given position (in line: " << lineNum
                        << ") exceeds the X/Y ship limits (ignored)\n";
            }
            result |= (1 << 1);
            continue;
        }
        if (indexVector[row][col] != -1) {
            if (indexVector[row][col] == actualFloors) {
                if (toWrite) {
                    outFile << "ship plan: bad line (" << lineNum
                            << ") format after first line - duplicate x,y appearance with same data (ignored)\n";
                }
                result |= (1 << 2);
            } else {
                outFile << "ship plan: travel error - duplicate x,y (in line: " << lineNum
                        << ") appearance with different data (cannot run this travel)\n";
                result |= (1 << 4);
            }
        } else {
            indexVector[row][col] = actualFloors;
            (*(*shipPtr)).initShipMapContainer(height - actualFloors, row, col);
        }
    }
    inFile.close();
    if (toWrite) { outFile.close(); }
    return result;
}


bool FileHandler::createCargoOpsFromFile(const string &fileName, list<shared_ptr<CargoOperation>> &ops,
                                         const string &errorFile) {
    bool result = true;
    ifstream inFile(fileName);
    ofstream outFile(errorFile, std::ios::app);
    bool toWrite = needToWrite(errorFile) && outFile;

    /*could not open file*/
    if (!inFile) {
        if (toWrite) {
            outFile << "Could not open operations file: " << fileName << "\n";
            outFile.close();
        }
        return false;
    }

    string line;
    int lineNum = 0;

    while (getline(inFile, line)) {
        lineNum++;
        if (line[0] == '#' || isLineEmpty(line)) { continue; }
        stringstream sline(line);
        vector<string> svec;
        string token;

        while (getline(sline, token, ',')) {
            token = trim(token);
            svec.push_back(token);
        }

        if (svec.size() <= 1 || svec[0].size() != 1) {
            if (toWrite) {
                outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
            }
            result = false;
            continue;
        }

        AbstractAlgorithm::Action action;
        shared_ptr<Container> cont =
                svec.size() > 1 ? std::make_shared<Container>(0, "", svec[1]) : std::make_shared<Container>(0, "", "");


        switch (svec[0].c_str()[0]) {
            case 'L':
                if (svec.size() == 5 && areNumbers(svec[2], svec[3], svec[4])) {
                    action = AbstractAlgorithm::Action::LOAD;
                    ops.emplace_back(
                            std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(svec[2]), stoi(svec[3]),
                                                                                    stoi(svec[4]))));
                } else {
                    result = false;
                    outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                }
                break;
            case 'M':
                if (svec.size() == 8) {
                    const string floor = svec[2], x = svec[3], y = svec[4], floorMove = svec[5], xMove = svec[6], yMove = svec[7];
                    if (areNumbers(floor, x, y) && areNumbers(floorMove, xMove, yMove)) {
                        action = AbstractAlgorithm::Action::MOVE;
                        ops.emplace_back(
                                std::make_shared<CargoOperation>(action, cont, MapIndex(stoi(floor), stoi(x),
                                                                                        stoi(y)),
                                                                 MapIndex(stoi(floorMove), stoi(xMove),
                                                                          stoi(yMove))));
                    } else {
                        result = false;
                        outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                    }
                } else {
                    result = false;
                    outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                }
                break;
            case 'R':
                if (svec.size() == 2) {
                    action = AbstractAlgorithm::Action::REJECT;
                    ops.emplace_back(std::make_shared<CargoOperation>(action, cont));
                } else {
                    result = false;
                    outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                }
                break;
            case 'U':
                if (svec.size() == 5 && areNumbers(svec[2], svec[3], svec[4])) {
                    action = AbstractAlgorithm::Action::UNLOAD;
                    ops.emplace_back(
                            std::make_shared<CargoOperation>(action, cont,
                                                             MapIndex(stoi(svec[2]), stoi(svec[3]),
                                                                      stoi(svec[4]))));
                } else {
                    result = false;
                    outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                }
                break;
            default:
                result = false;
                outFile << "Cargo Operation read error, line (" << lineNum << ") is in invalid format\n";
                break;
        }
    }

    inFile.close();
    if (!result) {
        outFile << "===========================================================================================\n";
    }
    if (toWrite) { outFile.close(); }
    return result;
}

void
FileHandler::reportPlanRouteErrors(const string &shipPlanPath, const string &routePath, const string &errorFile) {
    ofstream outFile(errorFile);
    if (!outFile) {
        return;
    }
    if (shipPlanPath.empty()) {
        outFile << "travel error - could not find only one .ship_plan file\n";
    }
    if (routePath.empty()) {
        outFile << "travel error - could not find only one .route file\n";
    }

    outFile.close();
}

void
FileHandler::printSimulatorResults(const string &filePath, list<string> &algoNameList, list<string> &travelNameList,
                                   unordered_map<string, unordered_map<string, int>> simulatorResultMap) {
    /*nothing to print*/
    if (simulatorResultMap.empty()) {
        return;
    }

    ofstream outFile(filePath);
    /*could not open file*/
    if (!outFile) {
        return;
    }

    for (auto i = algoNameList.begin(); i != algoNameList.end(); ++i) {
        /*print title*/
        if (i == algoNameList.begin()) {
            outFile << "RESULTS,    ";
            for (const string &travelName:travelNameList) {
                outFile << travelName << ",   ";
            }
            outFile << "Sum,    Num Errors\n";
        }

        int sumOps = 0, numErrors = 0;
        outFile << i->c_str() << ",    ";
        for (const string &travelName:travelNameList) {
            int ops = simulatorResultMap[i->c_str()][travelName];
            if (ops == -1) {
                numErrors++;
            } else {
                sumOps += ops;
            }
            outFile << ops << ",   ";
        }

        outFile << sumOps << ",    " << numErrors << "\n";

    }

    outFile.close();

}

void FileHandler::setUpErrorFiles(const string &outPath) {
    std::error_code er;
    string errorPath;
    if (!fs::exists(outPath, er)) {
        errorPath = fs::current_path(er).string() + "/errors";
    } else {
        errorPath = outPath + "/errors";
    }
    fs::remove_all(errorPath, er);
    fs::create_directories(errorPath, er);
}

string FileHandler::setCommandMap(unordered_map<string, string> &flagMap, char *argv[], int argc) {
    string errorString = "";
    for (int i = 1; i < argc; i++) {
        // if the command is a legal flag
        if (flagMap.find(argv[i]) != flagMap.end()) {
            if (argc >= i + 2) {
                // if value of flag is another flag
                if (flagMap.find(argv[i + 1]) != flagMap.end() || argv[i + 1][0] == '-') {
                    errorString += "Error: provided flag " + std::string(argv[i]) + " without value\n";
                } else {
                    flagMap[argv[i]] = argv[i + 1];
                    i++;
                }
            } else {
                errorString += "Error: provided flag " + std::string(argv[i]) + " without value\n";
            }
        } else {
            if (argv[i][0] == '-') {
                errorString += "Error: provided illegal flag " + std::string(argv[i]) + "\n";
            } else {
                errorString += "Error: provided illegal command " + std::string(argv[i]) + "\n";
            }
        }
    }
    return errorString;
}

bool FileHandler::canWriteinPath(const string &path) {
    ofstream tryToWrite(path + "/test");
    std::error_code er;
    if (!tryToWrite) {
        return false;
    }

    tryToWrite.close();
    fs::remove(path + "/test", er);
    return true;
}

