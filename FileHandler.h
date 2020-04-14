


#ifndef SHIPGIT_FILEHANDLER_H
#define SHIPGIT_FILEHANDLER_H

#include <string>
#include <list>
#include "Ship.h"
#include "CargoOperation.h"
using std::list;
using std::string;
#include "Container.h"
#include "SimulatorError.h"

class FileHandler {

public:
    /*returns empty list if invalid file or no legal containers*/
    static list<Container*> fileToContainerList(const string& fileName);
    static Ship* createShipFromFile(const string& fileName);
    static void operationsToFile(list<CargoOperation> operations, const string &fileName, const string& travelName, const string& currentPort, int visitNum);
    /*returns empty list if invalid file or no legal containers*/
    static list<string> fileToRouteList(const string& fileName);
    static void simulatorErrorsToFile(const list<SimulatorError>& simErrors, const string& fileName, const string& travelName, const string & portName = "",int visitNumber=0, bool noErrors = false);

};

#endif //SHIPGIT_FILEHANDLER_H
