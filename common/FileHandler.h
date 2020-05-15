/*class that handles input and output of different data objects to files and vice versa*/
#include <string>
#include <list>
#include "CargoOperation.h"
#include "Container.h"
#include "../simulator/SimulatorError.h"
#include <memory>
#include <unordered_map>
#include "../simulator/AlgorithmRegistrar.h"

using std::list;
using std::string;
using std::shared_ptr;
using std::unordered_map;

#ifndef SHIPGIT_FILEHANDLER_H
#define SHIPGIT_FILEHANDLER_H

class FileHandler {

public:
    /*appends containers to list*/
    static int fileToContainerList(const string &fileName, list<shared_ptr<Container>> &containerList,
                                   const string &errorFile = "", const string &portName = "");

    static int fileToRouteList(const string &fileName, list<string> &route, const string &errorFile = "");

    /*returns null ptr if ship was not created*/

    static int
    createShipMapFromFile(const string &fileName, shared_ptr<shared_ptr<ShipMap>> ship, const string &errorFile = "");

    static list<shared_ptr<CargoOperation>>
    createCargoOpsFromFile(const string &fileName);

    static void operationsToFile(list<CargoOperation> &operations, const string &fileName);

    /*returns empty list if invalid file or no legal containers*/

    static void reportPlanRouteErrors(const string &shipPlanPath, const string &routePath, const string &errorFile);

    static void printSimulatorResults(const string &filePath, list<string> &algoNameList, list<string> &travelNameList,
                                      unordered_map<string, unordered_map<string, int>> simulatorResultMap);

    static void setUpErrorFiles(const string &outputPath);

    static string setCommandMap(unordered_map<string, string> &flagMap, char *argv[], int argc);

    static void printAlgoRegistrationError(const string &fileName, const string &algoName,
                                           int result);

};

#endif //SHIPGIT_FILEHANDLER_H
