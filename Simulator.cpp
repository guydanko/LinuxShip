
#include "AbstractAlgorithm.h"
#include "Simulator.h"
#include "FileHandler.h"
#include "SimulatorAlgoCheck.h"

using std::string;

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


void setUpDirectories(const string &directoryRoot) {
    if (fs::exists(directoryRoot)) {
        fs::remove_all(directoryRoot);
    }
    fs::create_directory(directoryRoot);
    fs::create_directory(directoryRoot + "/" + "errors");
}

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
    list<shared_ptr<AbstractAlgorithm>> algoList;
    //register and push all algos (with names?)
    algoList.push_back(std::make_shared<NaiveStowageAlgorithm>());
//    algoList.push_back(std::make_shared<MoreNaiveAlgorithm>());
    for (auto algo: algoList) {
        for (Travel travel:this->travelList) {
            if (travel.isTravelLegal()) {
                this->algoXtravel.emplace_back(algo, travel);
            }
        }
    }
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
    travelList.emplace_back(path.string(), path.filename().string(), shipPlanPath, routePath, *shipPtr, route,
                            travelError);

}

int Simulator::initAlgoWithTravelParam(Travel &travel, shared_ptr<AbstractAlgorithm> pAlgo,
                                       list<SimulatorError> &errorList) {
    int algoInitError = 0;
    algoInitError |= pAlgo->readShipPlan(travel.getShipPlanPath());
    algoInitError |= pAlgo->readShipRoute(travel.getRoutePath());
    pAlgo->setWeightBalanceCalculator(calculator);
    return algoInitError;
}

int Simulator::runOneTravel(Travel &travel, shared_ptr<AbstractAlgorithm> pAlgo, const string &travelAlgoDirectory,
                            const string &errorFileName) {
    int algoInitError = 0;
    bool correctAlgo = true;
    int sumCargoOperation = 0;
    if (travel.isTravelLegal()) {
        list<SimulatorError> errorList;
        algoInitError = initAlgoWithTravelParam(travel, pAlgo, errorList);
        correctAlgo = SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError,
                                                                         travel.getTravelError(), errorList,correctAlgo);
        string travelErrorPath = this->outputPath + "/errors" + "/" + errorFileName;
        FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
        list<shared_ptr<Container>> doubleIdList = {};
        while (!travel.didTravelEnd() && correctAlgo) {
            set<string> rejectedID = {};
            int simulationInstError = 0;
            errorList = {};
            list<shared_ptr<Container>> loadList = {};
            simulationInstError |= travel.getContainerList(travelErrorPath, loadList);
            //path to read container list and write cargo op
            const string writeTo = travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +
                                   std::to_string(travel.getCurrentVisitNumber()) + ".crane_instructions";
            int algoGetInsError = pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
            list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo, loadList);
            sumCargoOperation += cargoOps.size();
            simulationInstError |= SimulatorAlgoCheck::connectContainerToCargoOp(loadList, travel.getShipMap(),
                                                                                 cargoOps, errorList,
                                                                                 doubleIdList, travel.getRoute(),
                                                                                 rejectedID, correctAlgo);
            simulationInstError |= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(),
                                                                        this->calculator, cargoOps, loadList,
                                                                        travel.getCurrentPort(), errorList,
                                                                        doubleIdList, rejectedID, correctAlgo);
            SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(algoGetInsError, simulationInstError, errorList,
                                                                   correctAlgo);
            FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName(),
                                               travel.getCurrentPort(), travel.getCurrentVisitNumber());
            travel.goToNextPort();
        }
        errorList = {};
        SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, correctAlgo);
        FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
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

void Simulator::run() {
    setUpDirectories(this->outputPath);
    createAlgoXTravel();
    int algoNum = 1;
    for (auto algoTravelPair: algoXtravel) {
        string fileName =
                this->outputPath + "/" + std::to_string(algoNum) + "_" + algoTravelPair.second.getTravelName();
        fs::create_directory(fileName);
        runOneTravel(algoTravelPair.second, algoTravelPair.first, fileName,
                     std::to_string(algoNum) + "_" + algoTravelPair.second.getTravelName());
        algoNum++;
    }
    deleteEmptyFiles();
}

