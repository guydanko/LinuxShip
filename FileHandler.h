/*class that handles input and output of different data objects to files and vice versa*/
#include <string>
#include <list>
#include "Ship.h"
#include "CargoOperation.h"
#include "Container.h"
#include "SimulatorError.h"
#include <memory>

using std::list;
using std::string;
using std::shared_ptr;

#ifndef SHIPGIT_FILEHANDLER_H
#define SHIPGIT_FILEHANDLER_H

class FileHandler {

public:
    /*returns empty list if invalid file or no legal containers*/
    static list<shared_ptr<Container>> fileToContainerList(const string& fileName,const string& errorFile = "");
    /*returns null ptr if ship was not created*/
    static Ship* createShipFromFile(const string& fileName, const string& errorFile = "");
    static list<shared_ptr<CargoOperation>> createCargoOpsFromFile(const string& fileName, list<shared_ptr<Container>>& containerList );
    static void operationsToFile(list<CargoOperation> operations, const string &fileName, const string& currentPort);
    /*returns empty list if invalid file or no legal containers*/
    static list<string> fileToRouteList(const string& fileName, const string& errorFile = "");
    static void simulatorErrorsToFile(const list<SimulatorError>& simErrors, const string& fileName, const string& travelName, const string & portName = "",int visitNumber=0, bool noErrors = false,const string& errorFile = "");

};

#endif //SHIPGIT_FILEHANDLER_H
