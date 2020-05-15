#include "SimulatorAlgoCheck.h"
#include <map>

using std::map;

void deleteRejectDoubleID(list<shared_ptr<CargoOperation>> &cargoOps, int countErase, list<SimulatorError> &listError,
                          const string &id, bool &correctAlgo) {
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
        correctAlgo = false;
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

void connectContainerFromShip(ShipMap *shipMap, list<shared_ptr<CargoOperation>> &cargoOps) {
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
                                cargoOp->setConnected(true);
                            }
                        }
                    }
                }
            }
        }
    }
}

void findRejectToDestNotInRoute(list<shared_ptr<Container>> &loadList, list<shared_ptr<CargoOperation>> &opList,
                                list<SimulatorError> &errorList, list<string> &route, set<string> &rejectedID,
                                bool &correctAlgo) {
    map<string, int> portNumberMap;

    int number = 1;
    auto itrPort = route.begin();
    itrPort++;
    for (; itrPort != route.end(); itrPort++) {
        auto itr = portNumberMap.find(*itrPort);
        if (itr == portNumberMap.end()) {
            portNumberMap[*itrPort] = number;
            number++;
        }
    }

    for (auto contItr = loadList.begin(); contItr != loadList.end();) {
        auto itrFind = portNumberMap.find((*contItr)->getDestination());
        // container destination is not in route or current destination- REJECT
        if (itrFind == portNumberMap.end() || (*contItr)->getDestination() == route.front()) {
            bool notFound = true;
            for (auto opItr = opList.begin(); opItr != opList.end() && notFound;) {
                if ((*opItr)->getOp() == AbstractAlgorithm::Action::REJECT &&
                    (*opItr)->getContainer()->getId() == (*contItr)->getId()) {
                    rejectedID.insert((*contItr)->getId());
                    contItr = loadList.erase(contItr);
                    opItr = opList.erase(opItr);
                    notFound = false;
                } else {
                    opItr++;
                }
            }
            if (notFound) {
                errorList.emplace_back("algorithm does not report REJECT to id- " + (*contItr)->getId() +
                                       " although it has destination not in route", SimErrorType::GENERAL_PORT);
                rejectedID.insert((*contItr)->getId());
                contItr = loadList.erase(contItr);
                correctAlgo = false;
            }
        } else {
            (*contItr)->setPortIndex(itrFind->second);
            contItr++;
        }
    }
}

void findRejectToIlligalContainer(list<shared_ptr<Container>> &loadList, list<shared_ptr<CargoOperation>> &opList,
                                  list<SimulatorError> &errorList, set<string> &rejectedID, bool &correctAlgo) {

    for (auto contItr = loadList.begin(); contItr != loadList.end();) {
        if (!(*contItr)->isContainerLegal()) {
            bool notFound = true;
            for (auto opItr = opList.begin(); opItr != opList.end() && notFound;) {
                if ((*opItr)->getOp() == AbstractAlgorithm::Action::REJECT &&
                    (*opItr)->getContainer()->getId() == (*contItr)->getId()) {
                    rejectedID.insert((*contItr)->getId());
                    contItr = loadList.erase(contItr);
                    opItr = opList.erase(opItr);
                    notFound = false;
                } else {
                    opItr++;
                }

            }
            if (notFound) {
                errorList.emplace_back("algorithm does not report REJECT to id- " + (*contItr)->getId() +
                                       " although it has illegal parameters", SimErrorType::GENERAL_PORT);
                rejectedID.insert((*contItr)->getId());
                contItr = loadList.erase(contItr);
                correctAlgo = false;
            }

        } else {
            contItr++;
        }
    }
}

void makeSureAllCargoOpConnected(list<shared_ptr<Container>> &loadList, list<shared_ptr<CargoOperation>> &opList,
                            list<SimulatorError> &errorList, set<string> &rejectedID, bool &correctAlgo) {
    for (auto & opItr : opList) {
        bool notFindCont = true;
        for (auto contItr = loadList.begin(); contItr != loadList.end() && notFindCont; contItr++) {
            if (opItr->getContainer()->getId() == (*contItr)->getId()) {
                opItr->setContainer(*contItr);
                opItr->setConnected(true);
                notFindCont = false;
            }
        }
    }
    for (auto opItr = opList.begin(); opItr != opList.end();) {
        if (!(*opItr)->getConnected()) {
            auto idRejectedItr = rejectedID.find((*opItr)->getContainer()->getId());
            if (idRejectedItr != rejectedID.end()) {
                if ((*opItr)->getOp() == AbstractAlgorithm::Action::REJECT) {
                    errorList.emplace_back("already gave REJECT operation, this operation is  unnecessary",
                                           SimErrorType::OPERATION_PORT, *(*opItr));
                    opItr = opList.erase(opItr);
                    correctAlgo = false;
                } else {
                    errorList.emplace_back(
                            "operation for container with id which has been REJECTED or should has been REJECTED - operation ignored",
                            SimErrorType::OPERATION_PORT,
                            *(*opItr));
                    opItr = opList.erase(opItr);
                    correctAlgo = false;
                }
            } else {
                errorList.emplace_back("use unknown id- operation ignored", SimErrorType::OPERATION_PORT, *(*opItr));
                opItr = opList.erase(opItr);
                correctAlgo = false;
            }
        } else {
            opItr++;
        }
    }
}

int SimulatorAlgoCheck::connectContainerToCargoOp(list<shared_ptr<Container>> &loadList, ShipMap *shipMap,
                                                  list<shared_ptr<CargoOperation>> &opList,
                                                  list<SimulatorError> &errorList,
                                                  list<shared_ptr<Container>> &doubleIdList, list<string> &route,
                                                  set<string> &rejectedID, bool &correctAlgo) {
    int result = 0;
    findRejectToIlligalContainer(loadList, opList, errorList, rejectedID, correctAlgo);
    findRejectToDestNotInRoute(loadList, opList, errorList, route, rejectedID, correctAlgo);
    //start take care of double id
    map<string, int> containerMap;
    /*create map of id to container in load list*/
    for (const auto &cont: loadList) {
        auto itr = containerMap.find(cont->getId());
        if (itr == containerMap.end()) {
            containerMap[cont->getId()] = 1;
        } else {
            containerMap[cont->getId()] += 1;
        }
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
            if (onShip) {
                result |= 1 << 11;
            } else {
                result |= 1 << 10;
            }
            deleteDoubleID(loadList, pair.first, onShip, doubleIdList);
            deleteRejectDoubleID(opList, pair.second - 1, errorList, pair.first, correctAlgo);
        }
    }
    connectContainerFromShip(shipMap, opList);
    makeSureAllCargoOpConnected(loadList,opList,errorList,rejectedID,correctAlgo);
    return result;
}

void
SimulatorAlgoCheck::checkIfShipEmpty(ShipMap *shipMap, list<SimulatorError> &errorList, bool &correctAlgo) {
    for (int i = 0; i < shipMap->getHeight(); i++) {
        for (int j = 0; j < shipMap->getRows(); j++) {
            for (int k = 0; k < shipMap->getCols(); k++) {
                if (shipMap->getShipMapContainer()[i][j][k] != nullptr &&
                    shipMap->getShipMapContainer()[i][j][k] != shipMap->getImaginary()) {
                    errorList.emplace_back(
                            "Container with id- " + shipMap->getShipMapContainer()[i][j][k]->getId() +
                            " is still on the ship at the end of the travel", SimErrorType::TRAVEL_END);
                    correctAlgo = false;
                }
            }
        }
    }
}

void SimulatorAlgoCheck::algoErrorInstVsSimulationErrorInst(int algoGetInsError, int simulationInstError,
                                                            list<SimulatorError> &errorList, bool &correctAlgo) {
    for (int i = 18; i >= 10; i--) {
        if ((algoGetInsError & (1 << i)) == (1 << i) && (simulationInstError & (1 << i)) == (1 << i)) {
            errorList.emplace_front("algorithm reports error code 2^" + std::to_string(i),
                                    SimErrorType::GENERAL_PORT);
        } else {
            if ((algoGetInsError & (1 << i)) != (1 << i) && (simulationInstError & (1 << i)) == (1 << i)) {
                errorList.emplace_front("simulation reports error code 2^" + std::to_string(i) +
                                        " but algorithm does not report this code too", SimErrorType::GENERAL_PORT);
                correctAlgo = false;
            } else {
                if ((algoGetInsError & (1 << i)) == (1 << i) && (simulationInstError & (1 << i)) != (1 << i)) {
                    errorList.emplace_front("algorithm reports error code 2^" + std::to_string(i) +
                                            " but simulation does not report this code too",
                                            SimErrorType::GENERAL_PORT);
                    correctAlgo = false;
                }
            }
        }
    }
}

bool SimulatorAlgoCheck::compareErrorAlgoSimulationInit(int algoInitError, int simulationInitError,
                                                        list<SimulatorError> &errorList, bool &correctAlgo) {
    bool canRun = true;
    string simulationAgree = "";
    for (int i = 1; i < 9; i++) {
        if ((algoInitError & (1 << i))) {
            if (!(simulationInitError & (1 << i))) {
                simulationAgree = ", but simulation does not find this error, algorithm incorrect";
                correctAlgo = false;
            }
            errorList.emplace_back(
                    "algorithm reports error 2^" + std::to_string(i) + simulationAgree,
                    SimErrorType::TRAVEL_INIT);
            simulationAgree = "";
            if (i == 3 || i == 4 || i == 7 || i == 8) {
                canRun = false;
            }
        }
    }
    if (!errorList.empty()) {
        errorList.emplace_front("errors reported by the algorithm while reading ship_plan and route:",
                                SimErrorType::TRAVEL_INIT);
        errorList.emplace_back("************************************************************************",
                               SimErrorType::TRAVEL_INIT);
    }
    return canRun;
}

bool indexInLimit(ShipMap *shipMap, MapIndex index) {
    if (index.getHeight() < 0 || index.getHeight() >= shipMap->getHeight()) {
        return false;
    }
    if (index.getRow() < 0 || index.getRow() >= shipMap->getRows()) {
        return false;
    }
    return !(index.getCol() < 0 || index.getCol() >= shipMap->getCols());
}

bool containerAbove(ShipMap *shipMap, MapIndex index) {
    for (int i = shipMap->getHeight() - 1; i > index.getHeight(); i--) {
        if (shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            return true;
        }
    }
    return false;
}

bool indexAccessible(ShipMap *shipMap, CargoOperation &cargoOp, list<SimulatorError> &errorList,
                     bool &correctAlgo) {
    if (!indexInLimit(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("illegal index, exceeds ship plan limits- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return false;
    }
    if (containerAbove(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("cannot reach container in index, blocked above by containers- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return false;
    }
    return true;
}

bool solidGround(ShipMap *shipMap, MapIndex index) {
    if (index.getHeight() == 0) {
        return true;
    }
    int i = index.getHeight() - 1;
    return shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr;
}

bool portStillInRoute(list<string> &route, const string &port) {
    auto itr = route.begin();
    itr++;
    for (; itr != route.end(); itr++) {
        if ((*itr) == port) {
            return true;
        }
    }
    return false;
}

void checkIfAllUnloaded(ShipMap *shipMap, const string &port, list<SimulatorError> &errorList,
                        list<shared_ptr<Container>> &doubleIdList, list<string> &route, bool &correctAlgo) {
    for (int i = 0; i < shipMap->getHeight(); i++) {
        for (int j = 0; j < shipMap->getRows(); j++) {
            for (int k = 0; k < shipMap->getCols(); k++) {
                if (shipMap->getShipMapContainer()[i][j][k] != nullptr &&
                    shipMap->getShipMapContainer()[i][j][k] != shipMap->getImaginary()) {
                    if (shipMap->getShipMapContainer()[i][j][k]->getDestination() == port) {
                        bool doubleIdProblem = false;
                        for (const auto& cont :doubleIdList) {
                            if (cont->getId() == shipMap->getShipMapContainer()[i][j][k]->getId()) {
                                if (portStillInRoute(route, cont->getDestination())) {
                                    errorList.emplace_back(
                                            "when load container with id- " + cont->getId() +
                                            " there was few container with same id. Algorithm rejects the first container and choose another container to load. It might cause earlier error such as weight..",
                                            SimErrorType::GENERAL_PORT);
                                    doubleIdProblem = true;
                                    correctAlgo = false;
                                    break;
                                }
                            }
                        }
                        if (!doubleIdProblem) {
                            errorList.emplace_back("Container on the ship with id- " +
                                                   shipMap->getShipMapContainer()[i][j][k]->getId() +
                                                   " has current destination -" + port +
                                                   "- but container still on the ship when ship left the port",
                                                   SimErrorType::GENERAL_PORT);
                            correctAlgo = false;
                        }
                    }
                }
            }
        }
    }
}

void checkLoadOperation(ShipMap *shipMap, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                        map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                        const string &currentPort,
                        int &maxNumberPortLoaded, list<SimulatorError> &errorList, bool &correctAlgo) {

    bool inLoadList = false, inRemember = false;
    bool access = indexAccessible(shipMap, cargoOp, errorList, correctAlgo);
    if (!access) {
        return;
    }
    if (shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] !=
        nullptr) {
        errorList.emplace_back(
                "index is occupied by container or cannot be place due to ship plan- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(true);
        correctAlgo = false;
        return;
    }
    if (!solidGround(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(true);
        correctAlgo = false;
        return;
    }
    for (const auto& pair: rememberToLoadAgainIdToCargoOp) {
        if (pair.first == cargoOp.getContainer()->getId()) {
            inRemember = true;
        }
    }
    for (const auto& container: loadList) {
        if (container->getId() == cargoOp.getContainer()->getId() && !container->getIsContainerLoaded()) {
            inLoadList = true;
        }
    }
    if (!inLoadList && !inRemember) {
        errorList.emplace_back(
                "container has been loaded before- operation ignored", SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;;
    }
    //load succeed
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = cargoOp.getContainer();
    cargoOp.getContainer()->setIsContainerLoaded(true);
    //load from remember
    if (inRemember) {
        shipMap->getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        auto itrRemember = rememberToLoadAgainIdToCargoOp.find(cargoOp.getContainer()->getId());
        if (itrRemember != rememberToLoadAgainIdToCargoOp.cend()) {
            rememberToLoadAgainIdToCargoOp.erase(itrRemember);
        }
    }
        //load from loadList
    else {
        shipMap->getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        if (cargoOp.getContainer()->getDestination() == currentPort) {
            errorList.emplace_back("should not load container with current port destination",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            correctAlgo = false;
            return;
        }
        if (cargoOp.getContainer()->getPortIndex() > maxNumberPortLoaded) {
            maxNumberPortLoaded = cargoOp.getContainer()->getPortIndex();
        }
    }
}

void checkUnloadOperation(ShipMap *shipMap, CargoOperation &cargoOp, const string &currentPort,
                          map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                          list<SimulatorError> &errorList, bool &correctAlgo) {
    bool access = indexAccessible(shipMap, cargoOp, errorList, correctAlgo);
    if (!access) {
        return;
    }
    shared_ptr<Container> containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == shipMap->getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        correctAlgo = false;
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;
    }
    //unload succeed
    shipMap->getContainerIDOnShip().erase(
            shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()]->getId());
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;
    //maybe future error
    if (cargoOp.getContainer()->getDestination() != currentPort) {
        cargoOp.getContainer()->setIsContainerLoaded(false);
        rememberToLoadAgainIdToCargoOp[cargoOp.getContainer()->getId()] = std::make_shared<CargoOperation>(cargoOp);
    }
}

void checkMoveOperation(ShipMap *shipMap, CargoOperation &cargoOp, list<SimulatorError> &errorList,
                        bool &correctAlgo) {
    bool accsees = indexAccessible(shipMap, cargoOp, errorList, correctAlgo);
    if (!accsees) {
        return;
    }
    shared_ptr<Container> containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == shipMap->getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        correctAlgo = false;
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;
    }
    //check place to move
    if (!indexInLimit(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("index to move is not in ship plan- operation ignored", SimErrorType::OPERATION_PORT,
                               cargoOp);
        correctAlgo = false;
        return;
    }
    if (containerAbove(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot reach move index, it is blocked by containers above it- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;
    }
    containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()];
    if (containerOnTheShipInThisPlace != nullptr) {
        errorList.emplace_back(
                "cannot move container to index occupied by other container, or space is not valid to this ship plan- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;
    }
    if (!solidGround(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return;
    }
    containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    //move succeed
    shipMap->getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()] = containerOnTheShipInThisPlace;
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;

}

int
checkRejectOperation(ShipMap *shipMap, CargoOperation &cargoOp, int maxNumberPortLoaded,
                     list<SimulatorError> &errorList, bool &correctAlgo) {
    int result = 0;
    if (cargoOp.getContainer()->getIsContainerLoaded()) {
        errorList.emplace_back(
                "already gave Load operation to this container, this operation is unnecessary and wrong- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return result;
    }
    cargoOp.getContainer()->setIsContainerReject(true);

    //destination in route
    MapIndex index = MapIndex::firstLegalIndexPlace(shipMap);
    if (index.validIndex()) {
        errorList.emplace_back(
                "rejected container with valid destination while there is still place on the ship",
                SimErrorType::OPERATION_PORT, cargoOp);
        correctAlgo = false;
        return result;
    } else {
        result |= 1 << 18;
        if (maxNumberPortLoaded > cargoOp.getContainer()->getPortIndex()) {
            errorList.emplace_back(
                    "rejected container with closer destination and loaded container with a further destination instead",
                    SimErrorType::OPERATION_PORT, cargoOp);
            correctAlgo = false;
            return result;
        }
    }
    return result;
}

//make sure nothing left on port with no reason
void nothingLeftNoReason(map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                         list<SimulatorError> &errorList,
                         const string &currentPort, list<shared_ptr<Container>> &doubleIdList, bool &correctAlgo) {
    for (auto pair : rememberToLoadAgainIdToCargoOp) {
        bool found = false;
        for (auto itr = doubleIdList.begin(); itr != doubleIdList.end() && !found; itr++) {
            if ((*itr)->getId() == pair.first && (*itr)->getDestination() == currentPort) {
                errorList.emplace_back(
                        "this container should not have been loaded. When it loaded there were few container with this id and algorithm rejects the first container and choose this container instead. It might cause earlier errors such as weight balance..",
                        SimErrorType::OPERATION_PORT, *pair.second);
                found = true;
                correctAlgo = false;
            }
        }
        if (!found) {
            errorList.emplace_back(
                    "unload container in port- " + currentPort + " instead of destination port " +
                    pair.second->getContainer()->getDestination() +
                    " ", SimErrorType::OPERATION_PORT, *pair.second);
            correctAlgo = false;
        }
    }
}

void checkAllContainersRejectedOrLoaded(list<shared_ptr<Container>> &loadList, list<SimulatorError> &errorList,
                                        bool &correctAlgo) {
    for (const auto& container: loadList) {
        if ( !container->getIsContainerLoaded()  && !container->getIsContainerReject()) {
            errorList.emplace_back("container id- " + container->getId() + " was not rejected or loaded",
                                   SimErrorType::GENERAL_PORT);
            correctAlgo = false;

        }
    }
}

bool checkBalance(CargoOperation *cargoOp, list<SimulatorError> &errorList, WeightBalanceCalculator &calculator) {
    AbstractAlgorithm::Action op = cargoOp->getOp();
    if (op == AbstractAlgorithm::Action::REJECT) {
        return true;
    }
    if (op == AbstractAlgorithm::Action::LOAD || op == AbstractAlgorithm::Action::UNLOAD) {
        if (calculator.tryOperation((char) op, cargoOp->getContainer()->getWeight(),
                                    cargoOp->getIndex().getCol(),
                                    cargoOp->getIndex().getRow()) !=
            WeightBalanceCalculator::BalanceStatus::APPROVED) {
            //TODO: calculator denied operation
            errorList.emplace_back("weight calculator does not approve this operation- operation ignored",
                                   SimErrorType::OPERATION_PORT, *cargoOp);
            return false;
        } else {
            return true;
        }
    }
    if (calculator.tryOperation('L', cargoOp->getContainer()->getWeight(),
                                cargoOp->getIndex().getCol(),
                                cargoOp->getIndex().getRow()) !=
        WeightBalanceCalculator::BalanceStatus::APPROVED ||
        calculator.tryOperation('U', cargoOp->getContainer()->getWeight(),
                                cargoOp->getMoveIndex().getCol(),
                                cargoOp->getMoveIndex().getRow()) !=
        WeightBalanceCalculator::BalanceStatus::APPROVED) {

        //TODO: calculator denied operation
        errorList.emplace_back("weight calculator does not approve this operation- operation ignored",
                               SimErrorType::OPERATION_PORT, *cargoOp);
        return false;
    } else {
        return true;
    }
}

int SimulatorAlgoCheck::checkAlgoCorrect(ShipMap *shipMap, list<string> &route,
                                         WeightBalanceCalculator &calculator,
                                         list<shared_ptr<CargoOperation>> &cargoOpsList,
                                         list<shared_ptr<Container>> &loadList,
                                         const string &currentPort,
                                         list<SimulatorError> &errorList, list<shared_ptr<Container>> &doubleIdList,
                                         bool &correctAlgo) {
    int result = 0;
    map<string, shared_ptr<CargoOperation>> rememberToLoadAgainIdToCargoOp;
    int maxNumberPortLoaded = 0;
    loadList.sort([](const shared_ptr<Container> cont1, const shared_ptr<Container> cont2) -> bool {
        return cont1->getPortIndex() < cont2->getPortIndex();
    });
    for (const auto& cargoOp: cargoOpsList) {
        if (!checkBalance(cargoOp.get(), errorList, calculator)) {
            correctAlgo = false;
        } else {
            AbstractAlgorithm::Action op = cargoOp->getOp();
            switch (op) {
                case AbstractAlgorithm::Action::LOAD:
                    checkLoadOperation(shipMap, *cargoOp, loadList, rememberToLoadAgainIdToCargoOp, currentPort,
                                       maxNumberPortLoaded, errorList, correctAlgo);
                    break;
                case AbstractAlgorithm::Action::UNLOAD:
                    checkUnloadOperation(shipMap, *cargoOp, currentPort, rememberToLoadAgainIdToCargoOp, errorList,
                                         correctAlgo);
                    break;
                case AbstractAlgorithm::Action::MOVE:
                    checkMoveOperation(shipMap, *cargoOp, errorList, correctAlgo);
                    break;
                case AbstractAlgorithm::Action::REJECT:
                    break;
            }
        }
    }
    for (const auto &cargoOp: cargoOpsList) {
        if (cargoOp->getOp() == AbstractAlgorithm::Action::REJECT) {
            result |= checkRejectOperation(shipMap, *cargoOp, maxNumberPortLoaded, errorList, correctAlgo);
        }
    }
    nothingLeftNoReason(rememberToLoadAgainIdToCargoOp, errorList, currentPort,
                        doubleIdList, correctAlgo); //make sure nothing left on port with no reason
    checkAllContainersRejectedOrLoaded(loadList, errorList, correctAlgo);
    checkIfAllUnloaded(shipMap, currentPort, errorList, doubleIdList, route, correctAlgo);

    return result;
}

