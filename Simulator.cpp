#include "AbstractAlgorithm.h"
#include "Simulator.h"
#include "FileHandler.h"
#include <map>
#include "SimulatorAlgoCheck.h"

using std::string;
using std::map;

void deleteRejectDoubleID(list<shared_ptr<CargoOperation>> &cargoOps, int countErase, list<SimulatorError> &listError,
                          const string &id, bool onShip) {
    size_t countRemoveOp = cargoOps.size();
    auto itr = cargoOps.begin();
    int count = countErase;
    while (countErase > 0 && itr != cargoOps.end()) {
        if ((*itr)->getContainer()->getId() == id && (*itr)->getOp() == AbstractAlgorithm::Action::REJECT) {
            itr = cargoOps.erase(itr);
            count--;
        } else {
            itr++;
        }
    }
    countRemoveOp -= cargoOps.size();
    if (count > 0) {
        listError.emplace_back("id- " + id + " rejected " + std::to_string(countRemoveOp) + " times but there are " +
                               std::to_string(countErase) + " container with this id to reject by reason of double id",
                               SimErrorType::GENERAL_PORT);
    }
}

void deleteDoubleID(list<shared_ptr<Container>> &loadList, const string &id, bool onShip,
                    list<shared_ptr<Container>> &doubleIdList) {
    //already on ship remove all with this id
    if (onShip) {
        loadList.remove_if([&id](shared_ptr<Container> &cont) { return cont->getId() == id; });
    }
        //not on ship remove all with this id except first
    else {
        auto startPlace = loadList.begin();
        while (startPlace != loadList.end() && (*startPlace)->getId() != id) {
            startPlace++;
        }
        if ((*startPlace)->getId() == id) {
            startPlace++;
        }
        for (auto itr = startPlace; itr != loadList.end();) {
            if ((*itr)->getId() == id) {
                doubleIdList.push_back(*itr);
                itr = loadList.erase(itr);
            } else {
                itr++;
            }
        }
    }
}

void connectContainerFromShip(shared_ptr<ShipMap> shipMap, list<shared_ptr<CargoOperation>> &cargoOps) {
    for (const auto &cargoOp: cargoOps) {
        auto inShip = shipMap->getContainerIDOnShip().find(cargoOp->getContainer()->getId());
        if (inShip != shipMap->getContainerIDOnShip().end()) {
            for (int i = 0; i < shipMap->getHeight(); i++) {
                for (int j = 0; j < shipMap->getRows(); j++) {
                    for (int k = 0; k < shipMap->getCols(); k++) {
                        if (shipMap->getShipMapContainer()[i][j][k] != nullptr) {
                            if (shipMap->getShipMapContainer()[i][j][k]->getId() ==
                                cargoOp->getContainer()->getId()) {
                                cargoOp->setContainer(shipMap->getShipMapContainer()[i][j][k]);
                            }
                        }
                    }
                }
            }
        }
    }
}

int connectContainerToCargoOp(list<shared_ptr<Container>> &loadList, shared_ptr<ShipMap> shipMap,
                          list<shared_ptr<CargoOperation>> &cargoOps,
                          list<SimulatorError> &listError, list<shared_ptr<Container>> &doubleIdList) {
    int result=0;
    map<string, int> containerMap;
    /*create map of id to container in load list*/
    for (const auto &cont: loadList) {
        containerMap[cont->getId()] = containerMap[cont->getId()] + 1;
    }

    //get for every id the specific number of container with id- load list + ship
    for (auto &pair : containerMap) {
        bool onShip = false;
        auto inShip = shipMap->getContainerIDOnShip().find(pair.first);
        if (inShip != shipMap->getContainerIDOnShip().end()) {
            pair.second++;
            onShip = true;
        }
        if (pair.second > 1) {
            if(onShip){
                result|= 1<<11;
            }
            else{
                result|= 1<<10;
            }

            deleteDoubleID(loadList, pair.first, onShip, doubleIdList);
            deleteRejectDoubleID(cargoOps, pair.second - 1, listError, pair.first, onShip);
        }
    }

    for (const auto &cargoOp: cargoOps) {
        for (const auto &cont : loadList) {
            if (cargoOp->getContainer()->getId() == cont->getId()) {
                cargoOp->setContainer(cont);
            }
        }
    }
    connectContainerFromShip(shipMap, cargoOps);
    return result;
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
        travel.errorsToFile(fileName);
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
    const string errorFileName = this->outputPath + "/errors/" + path.filename().string() +
                                 "_generalErrors";
    int routeError = FileHandler::fileToRouteList(path.string() + "/route", route, errorFileName);
    if (route.size() > 1) {
        auto shipPtr = std::make_shared<shared_ptr<ShipMap>>(std::make_shared<ShipMap>());
        int shipError = FileHandler::createShipMapFromFile(path.string() + "/shipPlan", shipPtr,
                                                           errorFileName);
        travelList.emplace_back(path.string(), path.filename().string(), *shipPtr, route, routeError, shipError);


    }

}
int Simulator::initAlgoWithTravelParam(Travel &travel,shared_ptr<AbstractAlgorithm> pAlgo,list<SimulatorError>& listError){
    int result=0;
    result|=pAlgo->readShipPlan(travel.getTravelPath() + "/shipPlan");
    result|= pAlgo->readShipRoute(travel.getTravelPath() + "/route");
    result|=  pAlgo->setWeightBalanceCalculator(calculator);
    return result;
}

void Simulator::runOneTravel(Travel &travel, shared_ptr<AbstractAlgorithm> pAlgo, const string &travelAlgoDirectory,
                             const string &errorFileName) {
    int simulatorInitError = 0; //should get the int number from build travel
    int algoInitError = 0;
    bool dramaticProblem=false; //should be the member of travel if it valid travel && the error the algo find
    list<SimulatorError> errorList;
    algoInitError =initAlgoWithTravelParam(travel, pAlgo, errorList);
    dramaticProblem|= SimulatorAlgoCheck::compareErrorAlgoSimulationInit(algoInitError,simulatorInitError,errorList);
    string travelErrorPath = this->outputPath + "/errors" + "/" + errorFileName;
    FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());

    int numberLoads = 0, numberUnloads = 0;
    while (!travel.didTravelEnd() && !dramaticProblem) {
        int simulationInstError=0;
        errorList = {};
        list<shared_ptr<Container>> loadList = {};
        list<shared_ptr<Container>> doubleIdList = {};
        simulationInstError += travel.getContainerList(travelErrorPath, loadList);
        if(!loadList.empty() && travel.getRoute().size()==1){
            simulationInstError|= 1<<17;
        }
        //path to read container list and write cargo op
        const string writeTo =travelAlgoDirectory + "/" + travel.getCurrentPort() + "_" +std::to_string(travel.getCurrentVisitNumber()) +".crane_instructions";
        int algoGetInsError = pAlgo->getInstructionsForCargo(travel.getNextCargoFilePath(), writeTo);
        list<shared_ptr<CargoOperation>> cargoOps = FileHandler::createCargoOpsFromFile(writeTo, loadList);
        simulationInstError|= connectContainerToCargoOp(loadList, travel.getShipMap(), cargoOps, errorList, doubleIdList);
        simulationInstError|= SimulatorAlgoCheck::checkAlgoCorrect(travel.getShipMap(), travel.getRoute(), this->calculator, cargoOps, loadList,
                                             travel.getCurrentPort(),
                                             numberLoads, numberUnloads, errorList, doubleIdList);
        SimulatorAlgoCheck::checkSameNumInstErrorAlgoVsSimulation(algoGetInsError,simulationInstError, errorList);
        FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName(),
                                           travel.getCurrentPort(), travel.getCurrentVisitNumber());
        travel.goToNextPort();
    }

    errorList = {};
    SimulatorAlgoCheck::checkIfShipEmpty(travel.getShipMap(), errorList, numberLoads, numberUnloads);
    FileHandler::simulatorErrorsToFile(errorList, travelErrorPath, travel.getTravelName());
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

