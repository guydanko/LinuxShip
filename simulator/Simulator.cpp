#include "../interfaces/AbstractAlgorithm.h"
#include "Simulator.h"
#include "../common/FileHandler.h"
#include "SimulatorAlgoCheck.h"
#include <map>
#include "AlgorithmRegistrar.h"

using std::string;
using std::multimap;

const string getShipPlanPath(const fs::path &path) {
    int planFiles = 0;
    string fileName;
    for (auto &p: fs::directory_iterator(path)) {
        if (p.path().filename().extension().string().compare(".ship_plan") == 0) {
            fileName = p.path().string();
            planFiles++;
        }
    }
    return planFiles == 1 ? fileName : "";
}

const string getRouteFilePath(const fs::path &path) {
    int routeFiles = 0;
    string fileName;
    for (auto &p: fs::directory_iterator(path)) {
        if (p.path().filename().extension().string().compare(".route") == 0) {
            fileName = p.path().string();
            routeFiles++;
        }
    }
    return routeFiles == 1 ? fileName : "";
}

//void Simulator::setUpDirectories() {
//    fs::remove_all(this->outputPath + "/" + "errors");
//    fs::create_directory(this->outputPath + "/" + "errors");
//}

void Simulator::travelErrorsToFile(const string &writeTo) {
    for (const Travel &travel:this->travelList) {
        travel.generalTravelErrorsToFile(writeTo + "/" + travel.getTravelName() + ".general_errors");
    }
}

void Simulator::createAlgoXTravel() {
    for (auto &p: fs::directory_iterator(this->travelPath)) {
        buildTravel(p);
    }
    travelErrorsToFile(this->outputPath + "/errors");

    for (auto &p: fs::directory_iterator(this->algoPath)) {
        if (p.path().extension().compare(".so") == 0) {
            AlgorithmRegistrar::getInstance().loadAlgorithm(p.path().string().c_str(), p.path().stem().string());
        }
    }
    this->algoList = AlgorithmRegistrar::getInstance().getAlgorithms();
}

void Simulator::buildTravel(const fs::path &path) {
    list<string> route = {};
    int travelError = 0;
    const string errorFileName = this->outputPath + "/errors/" + path.filename().string() +
                                 ".general_errors";
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

int Simulator::initAlgoWithTravelParam(Travel &travel, AbstractAlgorithm *pAlgo) {
    int algoInitError = 0;
    algoInitError |= pAlgo->readShipPlan(travel.getShipPlanPath());
    algoInitError |= pAlgo->readShipRoute(travel.getRoutePath());
    this->calculator.readShipPlan(travel.getShipPlanPath());
    pAlgo->setWeightBalanceCalculator(calculator);
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
        algoInitError = initAlgoWithTravelParam(travel, pAlgo);
        correctAlgo = SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError,
                                                                         travel.getTravelError(), errorList,
                                                                         correctAlgo);
        string travelErrorPath = this->outputPath + "/errors" + "/" + errorFileName;
        SimulatorError::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
        list<shared_ptr<Container>> doubleIdList = {};
        if (correctAlgo) {
            while (!travel.didTravelEnd()) {
                set<string> rejectedID = {};
                int simulationInstError = 0;
                errorList = {};
                list<shared_ptr<Container>> loadList = {};
                simulationInstError |= travel.getContainerList(travelErrorPath, loadList);
                //path to read container list and write cargo op
                const string writeTo = travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +
                                       std::to_string(travel.getCurrentVisitNumber()) + ".crane_instructions";
                int algoGetInsError = pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
                list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo);
                sumCargoOperation += cargoOps.size();
                simulationInstError |= SimulatorAlgoCheck::connectContainerToCargoOp(loadList, travel.getShipMap(),
                                                                                     cargoOps, errorList,
                                                                                     doubleIdList, travel.getRoute(),
                                                                                     rejectedID, correctAlgo);
                simulationInstError |= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(),
                                                                            this->calculator, cargoOps, loadList,
                                                                            travel.getCurrentPort(), errorList,
                                                                            doubleIdList, correctAlgo);
                SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(algoGetInsError, simulationInstError, errorList,
                                                                       correctAlgo);
                SimulatorError::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName(),
                                                      travel.getCurrentPort(), travel.getCurrentVisitNumber());
                travel.goToNextPort();
            }

            errorList = {};
            SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, correctAlgo);
            SimulatorError::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
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
        if (fs::file_size(simFiles) == 0) {
            fs::remove(simFiles);
        }
    }
    if (fs::is_empty(this->outputPath + "/errors")) {
        fs::remove_all(this->outputPath + "/errors");
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

    auto j = algoNames.begin();
    for (auto i = this->algoList.begin(); i != this->algoList.end(); ++i) {
        string algoName = j->c_str();
        j++;
        for (Travel travel: travelList) {
            string fileName = this->outputPath + "/" + algoName + "_" + travel.getTravelName();
            fs::create_directory(fileName);
            string errorFile = this->outputPath + "/errors/" + algoName + "_" + travel.getTravelName();
            int opAmount = runOneTravel(travel, i->get(), fileName, errorFile);
            algoOperationsMap[algoName][travel.getTravelName()] = opAmount;
        }
    }
    printResults(algoOperationsMap);
    deleteEmptyFiles();
}

