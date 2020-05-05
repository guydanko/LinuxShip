
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

void Simulator::travelErrorsToFile(const string &fileName) {
    for (const Travel &travel:this->travelList) {
        travel.generalTravelErrorsToFile(fileName);
    }
}

void Simulator::createAlgoXTravel() {
    for (auto &p: fs::directory_iterator(this->travelPath)) {
        buildTravel(p);
    }
    travelErrorsToFile(this->outputPath + "/errors");
    this->algoList.push_back(std::make_shared<NaiveStowageAlgorithm>());
    this->algoList.push_back(std::make_shared<MoreNaiveAlgorithm>());
    for (auto algo: algoList) {
        for (Travel travel:this->travelList) {
            this->algoXtravel.emplace_back(algo, travel);
        }
    }
}

void Simulator::buildTravel(const fs::path &path) {
    list<string> route = {};
    int travelError = 0;
    const string errorFileName = this->outputPath + "/errors/" + path.filename().string() +
                                 "_generalErrors";
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

int Simulator::initAlgoWithTravelParam(Travel &travel, shared_ptr<AbstractAlgorithm> pAlgo, list<SimulatorError> &errorList) {
    int algoInitError=0;
    algoInitError|= pAlgo->readShipPlan(travel.getShipPlanPath());
    algoInitError|= pAlgo->readShipRoute(travel.getRoutePath());
    pAlgo->setWeightBalanceCalculator(calculator);
    return algoInitError;
}

void Simulator::runOneTravel(Travel &travel, shared_ptr<AbstractAlgorithm> pAlgo, const string &travelAlgoDirectory,
                             const string &errorFileName) {
    int algoInitError = 0;
    if(travel.isTravelLegal()) {
        list<SimulatorError> errorList;
        algoInitError = initAlgoWithTravelParam(travel, pAlgo, errorList);
        bool algoInitSucceed= SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError, travel.getTravelError(), errorList);
        string travelErrorPath = this->outputPath + "/errors" + "/" + errorFileName;
        FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
        int numberLoads = 0, numberUnloads = 0;
        while (!travel.didTravelEnd() && algoInitSucceed) {
            int simulationInstError = 0;
            errorList = {};
            list<shared_ptr<Container>> loadList = {};
            list<shared_ptr<Container>> doubleIdList = {};
            set<string> rejectedID = {};
            simulationInstError |= travel.getContainerList(travelErrorPath, loadList);
            //path to read container list and write cargo op
            const string writeTo = travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +
                                   std::to_string(travel.getCurrentVisitNumber()) + ".crane_instructions";
            int algoGetInsError = pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
            list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo, loadList);
            simulationInstError |= SimulatorAlgoCheck::connectContainerToCargoOp(loadList, travel.getShipMap(), cargoOps, errorList,
                                                             doubleIdList, travel.getRoute(),rejectedID);
            simulationInstError |= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(),
                                                                        this->calculator, cargoOps, loadList,
                                                                        travel.getCurrentPort(),
                                                                        numberLoads, numberUnloads, errorList,
                                                                        doubleIdList,rejectedID);
            SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(algoGetInsError, simulationInstError, errorList);
            FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName(),
                                               travel.getCurrentPort(), travel.getCurrentVisitNumber());
            travel.goToNextPort();
        }
        errorList = {};
        SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, numberLoads, numberUnloads);
        FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());

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

