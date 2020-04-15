
#include "IncorrectAlgorithm.h"
#include "Simulator.h"
#include "FileHandler.h"
#include <map>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using std::string;
using std::map;

void setUpDirectories(const string& directoryRoot){
    if (fs::exists(directoryRoot)) {
        fs::remove_all(directoryRoot);
    }
    fs::create_directory(directoryRoot);
    fs::create_directory(directoryRoot + "/" + "Travel_File_Errors");
}

void Simulator::travelErrorsToFile(const string &fileName) {
    for (const Travel &travel:this->travelList) {
        travel.errorsToFile(fileName);
    }
}

Simulator::Simulator(const string &simulationDirectory) {
    setUpDirectories("SimulatorFiles");
    this->algoList.push_back(new NaiveStowageAlgorithm(nullptr, &calculator));
    this->algoList.push_back(new MoreNaiveAlgorithm(nullptr, &calculator));
    this->algoList.push_back(new IncorrectAlgorithm(nullptr, &calculator));
    int travelNum = 1;
    for (auto &p: fs::directory_iterator(simulationDirectory)) {
        const string path = p.path().string();
        Ship* ship = FileHandler::createShipFromFile(path+"/shipPlan.txt","SimulatorFiles/Travel_File_Errors/" +p.path().filename().string() +"FileErrors.txt" );
        if(ship != nullptr) {
            travelList.emplace_back(path, p.path().filename().string(), ship, "SimulatorFiles/Travel_File_Errors");
        }
        travelNum++;
    }
    travelErrorsToFile("SimulatorFiles/Travel_File_Errors");
    this->rootPath = simulationDirectory;


}

void freeAllContainers(list<list<Container *>> &containerList) {
    for (const auto &lst: containerList) {
        for (Container *container: lst) {
            delete container;
        }
    }
}


void Simulator::runOneTravel(Travel &travel, AbstractStowageAlgorithm *pAlgo, const string &fileName) {
    list<SimulatorError> listError;
    list<list<Container *>> allContainers;
    pAlgo->setShip(travel.getShip());
    Ship checkShip(*travel.getShip());
    string path = fileName + "/" + travel.getTravelName();
    fs::create_directory(path);
    int errorAmount = 0;
    while (!travel.didTravelEnd()) {
        list<Container *> loadList = travel.getContainerList(path);
        allContainers.push_back(loadList);
        list<CargoOperation> cargoOps = pAlgo->getInstructionsForCargo(loadList, travel.getShip()->getCurrentPort());
        FileHandler::operationsToFile(cargoOps, path, travel.getTravelName(), travel.getShip()->getCurrentPort(),
                                      travel.getCurrentVisitNumber());
        listError = checkAlgoCorrect(checkShip, cargoOps, loadList, travel.getShip()->getCurrentPort());
        errorAmount += listError.size();
        FileHandler::simulatorErrorsToFile(listError, path, travel.getTravelName(),
                                           travel.getShip()->getCurrentPort(), travel.getCurrentVisitNumber());
        travel.goToNextPort();
        checkShip.getShipRoute().pop_front();
    }
    travel.setToOriginalTravel();
    if (errorAmount == 0) {
        FileHandler::simulatorErrorsToFile(listError, path, travel.getTravelName(), "", 0, true);
    }
    freeAllContainers(allContainers);
}

void Simulator::run() {
    const string directoryRoot = "SimulatorFiles";
    for (AbstractStowageAlgorithm *pAlgo: algoList) {
        string fileName = directoryRoot + "/" + pAlgo->getName();
        fs::create_directory(fileName);
        for (Travel &travel: travelList) {
            runOneTravel(travel, pAlgo, fileName);
        }
    }
}

bool indexInLimit(Ship &ship, MapIndex index) {
    if (index.getHeight() < 0 || index.getHeight() >= ship.getShipMap().getHeight()) {
        return false;
    }
    if (index.getRow() < 0 || index.getRow() >= ship.getShipMap().getRows()) {
        return false;
    }
    return !(index.getCol() < 0 || index.getCol() >= ship.getShipMap().getCols());
}

bool containerAbove(Ship &ship, MapIndex index) {
    for (int i = ship.getShipMap().getHeight() - 1; i > index.getHeight(); i--) {
        if (ship.getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            return true;
        }
    }
    return false;
}

void indexAccessible(Ship &ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    if (!indexInLimit(ship, cargoOp.getIndex())) {
        errorList.emplace_back("illegal index, exceeds ship plan limits", 0, cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot reach container in index, blocked above by containers", 0, cargoOp);
        return;
    }
}

bool solidGround(Ship &ship, MapIndex index) {
    if (index.getHeight() == 0) {
        return true;
    }
    int i = index.getHeight() - 1;
    return ship.getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr;
}

void checkIfAllUnloaded(Ship &ship, const string &port, list<SimulatorError> &errorList) {
    for (int i = 0; i < ship.getShipMap().getHeight(); i++) {
        for (int j = 0; j < ship.getShipMap().getRows(); j++) {
            for (int k = 0; k < ship.getShipMap().getCols(); k++) {
                if (ship.getShipMap().getShipMapContainer()[i][j][k] != nullptr &&
                    ship.getShipMap().getShipMapContainer()[i][j][k] != ship.getShipMap().getImaginary()) {
                    if (ship.getShipMap().getShipMapContainer()[i][j][k]->getDestination() == port) {
                        errorList.emplace_back("Container on the ship with id- " +
                                               ship.getShipMap().getShipMapContainer()[i][j][k]->getId() +
                                               " has current destination (" + port + ") but unloaded", 1);
                        return;
                    }
                }
            }
        }
    }
}

bool compByPortDest(const Container *cont1, const Container *cont2) {
    return cont1->getPortIndex() < cont2->getPortIndex();
}

bool operationOrder(const SimulatorError &error1, const SimulatorError &error2) {
    return error1.getCargoOp().getPlaceInList() < error2.getCargoOp().getPlaceInList();
}

void orderListLoadContainer(list<Container *> &containerListToLoad, list<string> &route) {
    map<string, int> portNumberMap;
    int number = 1;
    for (const string &port : route) {
        auto itr = portNumberMap.find(port);
        if (itr == portNumberMap.end()) {
            portNumberMap[port] = number;
            number++;
        }
    }
    for (Container *cont : containerListToLoad) {
        auto itr = portNumberMap.find(cont->getDestination());
        // container destination is not in route- should reject
        if (itr == portNumberMap.end()) {
            cont->setPortIndex(0);
        } else {
            cont->setPortIndex(portNumberMap[itr->first]);
        }
    }
}

void checkLoadOperation(Ship &ship, CargoOperation &cargoOp, list<Container *> &loadList,
                        map<string, CargoOperation *> &rememberToLoadAgainIdToIndex, const string &currentPort,
                        int &maxNumberPortLoaded, list<SimulatorError> &errorList) {
    int numberOfIdInList = 0;
    Container *containerFromList = nullptr;
    indexAccessible(ship, cargoOp, errorList);
    if (ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] !=
        nullptr) {
        errorList.emplace_back("index is occupied by container or cannot be place due to ship plan", 0, cargoOp);
        return;
    }
    if (!solidGround(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot place container without underneath platform", 0, cargoOp);
        return;
    }
    for (auto pair: rememberToLoadAgainIdToIndex) {
        if (pair.first == cargoOp.getContainer()->getId()) {
            numberOfIdInList++;
        }
    }
    for (Container *container: loadList) {
        if (container->getId() == cargoOp.getContainer()->getId() && container->getIsContainerLoaded() != 1) {
            numberOfIdInList++;
            containerFromList = container;
        }
    }
    if (numberOfIdInList <= 0) {
        errorList.emplace_back("container with this id does not exist in port", 0, cargoOp);
        return;;
    }
    //load succeed
    ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = cargoOp.getContainer();
    cargoOp.getContainer()->setIsContainerLoaded(1);
    //load from remember
    if (containerFromList == nullptr) {
        auto itrRemember = rememberToLoadAgainIdToIndex.find(cargoOp.getContainer()->getId());
        if (itrRemember != rememberToLoadAgainIdToIndex.cend()) {
            rememberToLoadAgainIdToIndex.erase(itrRemember);
        }
    }
        //load from loadList
    else {
        auto itr = ship.getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
        if (itr != ship.getShipMap().getContainerIDOnShip().cend()) {
            errorList.emplace_back("container with this id is already on the ship", 0, cargoOp);
            return;
        }
        ship.getShipMap().getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        if (cargoOp.getContainer()->getDestination() == currentPort) {
            errorList.emplace_back("cannot load container with current port destination", 0, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() == 0) {
            errorList.emplace_back("cannot load container with destination-" + containerFromList->getDestination() +
                                   " which is not in the ship's route", 0, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() > maxNumberPortLoaded) {
            maxNumberPortLoaded = containerFromList->getPortIndex();
        }
    }
}

void checkUnloadOperation(Ship &ship, CargoOperation &cargoOp, const string &currentPort,
                          map<string, CargoOperation *> &rememberToLoadAgainIdToIndex,
                          list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    Container *containerOnTheShipInThisPlace = ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr || containerOnTheShipInThisPlace == ship.getShipMap().getImaginary()) {
        errorList.emplace_back("no container in this index to unload or this index is not part of the ship plan", 0,
                               cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship", 0, cargoOp);
        return;
    }
    //unload succeed
    ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;
    //maybe future error
    if (cargoOp.getContainer()->getDestination().compare(currentPort) != 0) {
        cargoOp.getContainer()->setIsContainerLoaded(0);
        rememberToLoadAgainIdToIndex[cargoOp.getContainer()->getId()] = &cargoOp;
    }
}

void checkMoveOperation(Ship &ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    Container *containerOnTheShipInThisPlace = ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr || containerOnTheShipInThisPlace == ship.getShipMap().getImaginary()) {
        errorList.emplace_back("no container in this index to unload or this index is not part of the ship plan", 0,
                               cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship", 0, cargoOp);
        return;
    }
    //check place to move
    if (!indexInLimit(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("index to move is not in ship plan", 0, cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot reach move index, it is blocked by containers above it", 0, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship.getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()];
    if (containerOnTheShipInThisPlace != nullptr) {
        errorList.emplace_back(
                "cannot move container to index occupied by other container, or space is not valid to this ship plan",
                0, cargoOp);
        return;
    }
    if (!solidGround(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot place container without underneath platform", 0, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    //move succeed
    ship.getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()] = containerOnTheShipInThisPlace;
    ship.getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;

}

void checkRejectOperation(Ship &ship, CargoOperation &cargoOp, list<Container *> &loadList, int maxNumberPortLoaded,
                          list<SimulatorError> &errorList, const string &currentPort) {
    cargoOp.getContainer()->setIsContainerReject(1);
    //container destination is here no reason to load
    if (cargoOp.getContainer()->getDestination() == currentPort) {
        return;
    }
    //container illegal should reject
    if (!cargoOp.getContainer()->isContainerLegal()) {
        return;
    }
    int numIdInLoadList = 0;
    for (Container *cont :loadList) {
        if (cont->getId() == cargoOp.getContainer()->getId()) {
            numIdInLoadList++;
        }
    }
    if (numIdInLoadList == 0) {
        errorList.emplace_back("rejected container with id which is not in the container list to load in this port", 0,
                               cargoOp);
        return;
    }
    //check container Id on ship
    auto itr = ship.getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
    if (itr == ship.getShipMap().getContainerIDOnShip().cend()) {
        //destination in route
        if (cargoOp.getContainer()->getPortIndex() > 0) {
            MapIndex index = MapIndex::firstLegalIndexPlace(ship.getShipMap());
            if (index.validIndex()) {
                errorList.emplace_back(
                        "rejected container with valid destination while there is still place on the ship", 0, cargoOp);
                return;
            } else {
                if (maxNumberPortLoaded > cargoOp.getContainer()->getPortIndex()) {
                    errorList.emplace_back(
                            "rejected container with closer destination and loaded container with a further destination instead",
                            0, cargoOp);
                    return;
                }
            }
        }
    }
}

//make sure nothing left on port with no reason
void nothingLeftNoReason(map<string, CargoOperation *> &rememberToLoadAgainIdToIndex, list<SimulatorError> &errorList,
                         const string &currentPort) {
    for (auto pair : rememberToLoadAgainIdToIndex) {
        errorList.emplace_back(
                "unload container in port- " + currentPort + " instead of destination port " +
                pair.second->getContainer()->getDestination() +
                " ", 0, *pair.second);
    }
}

void checkAllContainersRejectedOrLoaded(list<Container *> &loadList, list<SimulatorError> &errorList) {
    for (Container *container: loadList) {
        if (container->getIsContainerLoaded() == 0 && container->getIsContainerReject() == 0) {
            errorList.emplace_back("container id- " + container->getId() + " was not rejected or loaded", 1);
            break;
        }
    }
}

list<SimulatorError>
Simulator::checkAlgoCorrect(Ship &ship, list<CargoOperation> &cargoOpsList, list<Container *> &loadList,
                            const string &currentPort) {
    list<SimulatorError> errorList;
    map<string, CargoOperation *> rememberToLoadAgainIdToIndex;
    int number = 1;
    int maxNumberPortLoaded = 0;
    orderListLoadContainer(loadList, ship.getShipRoute());
    loadList.sort(compByPortDest);
    for (CargoOperation &cargoOp: cargoOpsList) {
        cargoOp.setPlaceInList(number);
        number++;
        Operation op = cargoOp.getOp();
        if (this->calculator.tryOperation() != BalanceStatus::APPROVED) {
            //TODO: calculator denied operation
            errorList.emplace_back("weight calculator does not approve this operation", 0, cargoOp);
        } else {
            switch (op) {
                case Operation::LOAD:
                    checkLoadOperation(ship, cargoOp, loadList, rememberToLoadAgainIdToIndex, currentPort,
                                       maxNumberPortLoaded, errorList);
                    break;
                case Operation::UNLOAD:
                    checkUnloadOperation(ship, cargoOp, currentPort, rememberToLoadAgainIdToIndex, errorList);
                    break;
                case Operation::MOVE:
                    checkMoveOperation(ship, cargoOp, errorList);
                    break;
                case Operation::REJECT:
                    break;
                default:
                    //TODO: deal with operation which not CargoOperation - maybe in targil 2
                    break;
            }
        }
    }
    for (CargoOperation &cargoOp: cargoOpsList) {
        if (cargoOp.getOp() == Operation::REJECT) {
            checkRejectOperation(ship, cargoOp, loadList, maxNumberPortLoaded, errorList, currentPort);
        }
    }
    nothingLeftNoReason(rememberToLoadAgainIdToIndex, errorList,
                        currentPort); //make sure nothing left on port with no reason
    errorList.sort(operationOrder); //sort by operation order in list
    checkAllContainersRejectedOrLoaded(loadList, errorList);
    checkIfAllUnloaded(ship, currentPort, errorList);
    return errorList;
}

Simulator::~Simulator() {
    for (AbstractStowageAlgorithm *algo : algoList) {
        delete algo;
    }
}