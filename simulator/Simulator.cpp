#include "../interfaces/AbstractAlgorithm.h"
#include "Simulator.h"
#include "../common/FileHandler.h"
#include "SimulatorAlgoCheck.h"
#include <map>
#include "AlgorithmRegistrar.h"
#include "../algorithms/_316294636_a.h"

using std::string;
using std::multimap;

string getShipPlanPath(const fs::path &path) {
    int planFiles = 0;
    string fileName;
    for (auto &p: fs::directory_iterator(path)) {
        if (p.path().filename().extension().string() == ".ship_plan") {
            fileName = p.path().string();
            planFiles++;
        }
    }
    return planFiles == 1 ? fileName : "";
}

string getRouteFilePath(const fs::path &path) {
    int routeFiles = 0;
    string fileName;
    for (auto &p: fs::directory_iterator(path)) {
        if (p.path().filename().extension().string() == ".route") {
            fileName = p.path().string();
            routeFiles++;
        }
    }
    return routeFiles == 1 ? fileName : "";
}

void Simulator::travelErrorsToFile(const string &writeTo) {
    for (const Travel &travel:this->travelList) {
        travel.generalTravelErrorsToFile(writeTo + "/" + travel.getTravelName() + "_general.errors");
    }
}

void Simulator::cleanFiles(list<string> &algoNames) {
    for (auto &algoName : algoNames) {
        for (Travel travel: travelList) {
            fs::remove_all(this->outputPath + "/" + algoName + "_" + travel.getTravelName() + "_crane_instructions");
        }
    }
    fs::remove(this->outputPath + "/simulation.results");
}

void Simulator::createAlgoList() {
    for (auto &p: fs::directory_iterator(this->algoPath)) {
        if (p.path().extension().compare(".so") == 0) {
            int result = AlgorithmRegistrar::getInstance().loadAlgorithm(p.path().string().c_str(),
                                                                         p.path().stem().string());
            AlgorithmRegistrar::printAlgoRegistrationError(outputPath + "/errors/algoRegistration.errors",
                                                    p.path().stem().string(), result);
        }
    }
    this->algoList = AlgorithmRegistrar::getInstance().getAlgorithms();
}

void Simulator::createAlgoXTravel() {
    for (auto &p: fs::directory_iterator(this->travelPath)) {
        if (fs::is_directory(p)) { buildTravel(p); }
    }
    travelErrorsToFile(this->outputPath + "/errors");
    createAlgoList();
}

void Simulator::buildTravel(const fs::path &path) {
    list<string> route = {};
    int travelError = 0;
    const string errorFileName = this->outputPath + "/errors/" + path.filename().string() + "_general.errors";
    string shipPlanPath = getShipPlanPath(path), routePath = getRouteFilePath(path);;
    if (shipPlanPath.empty() || routePath.empty()) {
        FileHandler::reportPlanRouteErrors(shipPlanPath, routePath, errorFileName);
        return;
    }

    auto shipPtr = std::make_shared<shared_ptr<ShipMap>>(std::make_shared<ShipMap>());
    travelError |= FileHandler::createShipMapFromFile(shipPlanPath, shipPtr,
                                                      errorFileName);
    travelError |= FileHandler::fileToRouteList(routePath, route, errorFileName);
    if (Travel::isTravelErrorLegal(travelError)) {
        travelList.emplace_back(path.string(), path.filename().string(), shipPlanPath, routePath, *shipPtr, route,
                                travelError);
    }

}

int Simulator::initAlgoWithTravelParam(Travel &travel, AbstractAlgorithm *pAlgo, list<SimulatorError> &errorList,
                                       bool &correctAlgo) {
    unsigned int algoInitError = 0;
    try {
        algoInitError |= pAlgo->readShipPlan(travel.getShipPlanPath());
        algoInitError |= pAlgo->readShipRoute(travel.getRoutePath());
        this->calculator.readShipPlan(travel.getShipPlanPath());
        pAlgo->setWeightBalanceCalculator(calculator);
    }
    catch (...) {
        errorList.emplace_front(
                "algorithm throw exception in init function (readShipPlan/readShipRoute/setWeightBalanceCalculator. Can't run this algorithm on this travel",
                SimErrorType::TRAVEL_INIT);
        correctAlgo = false;
    }
     return algoInitError;
}

/* returns amount of operations in a travel algo pair*/
int Simulator::runOneTravel(Travel &travel, AbstractAlgorithm *pAlgo, const string &travelAlgoDirectory,
                            const string &errorFileName) {
    int algoInitError = 0;
    bool correctAlgo = true;
    int sumCargoOperation = 0;
    if (travel.isTravelLegal()) {
        list<SimulatorError> errorList;
        algoInitError = initAlgoWithTravelParam(travel, pAlgo, errorList, correctAlgo);
        if (correctAlgo) {
            correctAlgo = SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError,
                                                                             travel.getTravelError(), errorList,
                                                                             correctAlgo);
        }
        SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName());
        list<shared_ptr<Container>> doubleIdList = {};
        if (correctAlgo) {
            bool throwException = false;
            while (!travel.didTravelEnd() && !throwException) {
                set<string> rejectedID = {};
                int simulationInstError = 0;
                errorList = {};
                list<shared_ptr<Container>> loadList = {};
                simulationInstError |= travel.getContainerList(errorFileName, loadList);
                //path to read container list and write cargo op
                const string writeTo = travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +
                                       std::to_string(travel.getCurrentVisitNumber()) + ".crane_instructions";
                int algoGetInsError = 0;
                try {
                    algoGetInsError = pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
                }
                catch (...) {
                    throwException = true;
                }
                if (!throwException) {
                    list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo);
                    sumCargoOperation += cargoOps.size();
                    simulationInstError |= SimulatorAlgoCheck::connectContainerToCargoOp(loadList, travel.getShipMap(),
                                                                                         cargoOps, errorList,
                                                                                         doubleIdList,
                                                                                         travel.getRoute(),
                                                                                         rejectedID, correctAlgo);
                    simulationInstError |= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(),
                                                                                this->calculator, cargoOps, loadList,
                                                                                travel.getCurrentPort(), errorList,
                                                                                doubleIdList, correctAlgo);
                    SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(algoGetInsError, simulationInstError,
                                                                           errorList,
                                                                           correctAlgo);
                    SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName(),
                                                          travel.getCurrentPort(), travel.getCurrentVisitNumber());
                    travel.goToNextPort();
                }
            }
            if (!throwException) {
                errorList = {};
                SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, correctAlgo);
                SimulatorError::simulatorErrorsToFile(errorList, errorFileName, travel.getTravelName());
            }
        }
    }
    if (correctAlgo) {
        return sumCargoOperation;
    } else {
        return -1;
    }


}

void Simulator::deleteEmptyFiles() {
    for (auto &simFiles: fs::directory_iterator(this->outputPath + "/errors")) {
        const string file = simFiles.path().string();
        if (fs::file_size(simFiles) == 0) {
            fs::remove(simFiles);
        }
    }
    if (fs::is_empty(this->outputPath + "/errors")) {
        fs::remove_all(this->outputPath + "/errors");
    }
    if (fs::exists(this->outputPath + "/simulation.results") &&
        fs::is_empty(this->outputPath + "/simulation.results")) {
        fs::remove_all(this->outputPath + "/simulation.results");
    }
}

list<string> getAlgosByOrder(unordered_map<string, unordered_map<string, int>> &algoOpMap) {
    list<string> algosInOrder;
    multimap<pair<int, int>, string> algoSumErrorMap;
    for (auto &algo: algoOpMap) {
        int algoErrors = 0, algoSum = 0;
        for (auto &travelName: algo.second) {
            if (travelName.second == -1) {
                algoErrors++;
            } else {
                algoSum += travelName.second;
            }
        }
        algoSumErrorMap.insert(std::make_pair(std::make_pair(algoErrors, algoSum), algo.first));
    }
    for (auto &pair:algoSumErrorMap) {
        algosInOrder.emplace_back(pair.second);
    }
    return algosInOrder;
}

void Simulator::printResults(unordered_map<string, unordered_map<string, int>> simResults) {
    list<string> algosInOrder = getAlgosByOrder(simResults);
    list<string> travelNameOrder = {};
    for (auto &travel: this->travelList) {
        travelNameOrder.emplace_back(travel.getTravelName());
    }
    FileHandler::printSimulatorResults(this->outputPath + "/simulation.results", algosInOrder, travelNameOrder,
                                       simResults);
}

void Simulator::run() {
    createAlgoXTravel();
    unordered_map<string, unordered_map<string, int>> algoOperationsMap;
    list<string> algoNames = AlgorithmRegistrar::getInstance().getAlgorithmNames();
    cleanFiles(algoNames);
    auto currentAlgoName = algoNames.begin();
    for (auto &pAlgo : this->algoList) {
        string algoName = *currentAlgoName;
        currentAlgoName++;
        for (Travel travel: travelList) {
            string fileName = this->outputPath + "/" + algoName + "_" + travel.getTravelName() + "_crane_instructions";
            fs::create_directory(fileName);
            string errorFile = this->outputPath + "/errors/" + algoName + "_" + travel.getTravelName() + ".errors";
            int opAmount = runOneTravel(travel, pAlgo.get(), fileName, errorFile);
            algoOperationsMap[algoName][travel.getTravelName()] = opAmount;
        }
    }
    printResults(algoOperationsMap);
    deleteEmptyFiles();
}

//void Simulator::runWindows() {
//    createAlgoXTravel();
//    unordered_map<string, unordered_map<string, int>> algoOperationsMap;
//    for (auto i = this->algoList.begin(); i != this->algoList.end(); ++i) {
//        for (Travel travel: travelList) {
//            string fileName = this->outputPath + "/" + "algo" + "_" + travel.getTravelName();
//            fs::create_directory(fileName);
//            string errorFile = this->outputPath + "/errors/" + "algo" + "_" + travel.getTravelName() + ".errors";
//            int opAmount = runOneTravel(travel, i->get(), fileName, errorFile);
//            algoOperationsMap["algo"][travel.getTravelName()] = opAmount;
//        }
//    }
//    printResults(algoOperationsMap);
//    deleteEmptyFiles();
//}


