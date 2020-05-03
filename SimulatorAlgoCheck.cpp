#include "SimulatorAlgoCheck.h"
#include <map>

using std::map;

bool indexInLimit(shared_ptr<ShipMap> shipMap, MapIndex index) {
    if (index.getHeight() < 0 || index.getHeight() >= shipMap->getHeight()) {
        return false;
    }
    if (index.getRow() < 0 || index.getRow() >= shipMap->getRows()) {
        return false;
    }
    return !(index.getCol() < 0 || index.getCol() >= shipMap->getCols());
}

bool containerAbove(shared_ptr<ShipMap> shipMap, MapIndex index) {
    for (int i = shipMap->getHeight() - 1; i > index.getHeight(); i--) {
        if (shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            return true;
        }
    }
    return false;
}

void indexAccessible(shared_ptr<ShipMap> shipMap, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    if (!indexInLimit(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("illegal index, exceeds ship plan limits- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (containerAbove(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("cannot reach container in index, blocked above by containers- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
}

bool solidGround(shared_ptr<ShipMap> shipMap, MapIndex index) {
    if (index.getHeight() == 0) {
        return true;
    }
    int i = index.getHeight() - 1;
    return shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr;
}

void checkIfAllUnloaded(shared_ptr<ShipMap> shipMap, const string &port, list<SimulatorError> &errorList) {
    for (int i = 0; i < shipMap->getHeight(); i++) {
        for (int j = 0; j < shipMap->getRows(); j++) {
            for (int k = 0; k < shipMap->getCols(); k++) {
                if (shipMap->getShipMapContainer()[i][j][k] != nullptr &&
                    shipMap->getShipMapContainer()[i][j][k] != shipMap->getImaginary()) {
                    if (shipMap->getShipMapContainer()[i][j][k]->getDestination() == port) {
                        errorList.emplace_back("Container on the ship with id- " +
                                               shipMap->getShipMapContainer()[i][j][k]->getId() +
                                               " has current destination -" + port +
                                               "- but container still on the ship when ship left the port",
                                               SimErrorType::GENERAL_PORT);
                        return;
                    }
                }
            }
        }
    }
}

bool compByPortDest(const shared_ptr<Container> cont1, const shared_ptr<Container> cont2) {
    return cont1->getPortIndex() < cont2->getPortIndex();
}

void orderListLoadContainer(list<shared_ptr<Container>> &containerListToLoad, list<string> &route) {
    map<string, int> portNumberMap;
    int number = 1;
    for (const string &port : route) {
        auto itr = portNumberMap.find(port);
        if (itr == portNumberMap.end()) {
            portNumberMap[port] = number;
            number++;
        }
    }
    for (auto cont : containerListToLoad) {
        auto itr = portNumberMap.find(cont->getDestination());
        // container destination is not in route- should reject
        if (itr == portNumberMap.end()) {
            cont->setPortIndex(0);
        } else {
            cont->setPortIndex(portNumberMap[itr->first]);
        }
    }
}

void checkLoadOperation(shared_ptr<ShipMap> shipMap, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                        map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                        const string &currentPort,
                        int &maxNumberPortLoaded, list<SimulatorError> &errorList) {

    if (!cargoOp.getContainer()->isContainerLegal()) {
        errorList.emplace_back("load container with illegal parameters- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        //although operation wants to load container with  illegal parameters, algo still gives an operation to this container
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    int numberOfIdInList = 0;
    shared_ptr<Container> containerFromLoadList = nullptr;
    indexAccessible(shipMap, cargoOp, errorList);
    if (shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] !=
        nullptr) {
        errorList.emplace_back("index is occupied by container or cannot be place due to ship plan- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    if (!solidGround(shipMap, cargoOp.getIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    for (auto pair: rememberToLoadAgainIdToCargoOp) {
        if (pair.first == cargoOp.getContainer()->getId()) {
            numberOfIdInList++;
        }
    }
    for (auto container: loadList) {
        if (container->getId() == cargoOp.getContainer()->getId() && container->getIsContainerLoaded() != 1) {
            numberOfIdInList++;
            containerFromLoadList = cargoOp.getContainer();
        }
    }
    if (numberOfIdInList <= 0) {
        errorList.emplace_back(
                "container with this id does not exist in port. It might have been loaded before- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;;
    }
    //load succeed
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = cargoOp.getContainer();
    cargoOp.getContainer()->setIsContainerLoaded(1);
    //load from remember
    if (containerFromLoadList == nullptr) {
        shipMap->getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        auto itrRemember = rememberToLoadAgainIdToCargoOp.find(cargoOp.getContainer()->getId());
        if (itrRemember != rememberToLoadAgainIdToCargoOp.cend()) {
            rememberToLoadAgainIdToCargoOp.erase(itrRemember);
        }
    }
        //load from loadList
    else {
        auto itr = shipMap->getContainerIDOnShip().find(cargoOp.getContainer()->getId());
        if (itr != shipMap->getContainerIDOnShip().cend()) {
            errorList.emplace_back("should not load container because this id is already on the ship",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        shipMap->getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        if (cargoOp.getContainer()->getDestination() == currentPort) {
            errorList.emplace_back("should not load container with current port destination",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromLoadList->getPortIndex() == 0) {
            errorList.emplace_back(
                    "should not load container with destination-" + containerFromLoadList->getDestination() +
                    " which is not in the ship's route", SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromLoadList->getPortIndex() > maxNumberPortLoaded) {
            maxNumberPortLoaded = containerFromLoadList->getPortIndex();
        }
    }
}

void checkUnloadOperation(shared_ptr<ShipMap> shipMap, CargoOperation &cargoOp, const string &currentPort,
                          map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                          list<SimulatorError> &errorList) {
    indexAccessible(shipMap, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == shipMap->getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    //unload succeed
    shipMap->getContainerIDOnShip().erase(
            shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()]->getId());
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;
    //maybe future error
    if (cargoOp.getContainer()->getDestination() != currentPort) {
        cargoOp.getContainer()->setIsContainerLoaded(0);
        rememberToLoadAgainIdToCargoOp[cargoOp.getContainer()->getId()] = std::make_shared<CargoOperation>(cargoOp);
    }
}

void checkMoveOperation(shared_ptr<ShipMap> shipMap, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    indexAccessible(shipMap, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == shipMap->getImaginary()) {
        errorList.emplace_back(
                "no container in this index to unload or this index is not part of the ship plan- operation ignored",
                SimErrorType::OPERATION_PORT,
                cargoOp);
        return;
    }
    if (containerOnTheShipInThisPlace->getId() != cargoOp.getContainer()->getId()) {
        errorList.emplace_back("the container id does not match container id on the ship- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    //check place to move
    if (!indexInLimit(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("index to move is not in ship plan- operation ignored", SimErrorType::OPERATION_PORT,
                               cargoOp);
        return;
    }
    if (containerAbove(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot reach move index, it is blocked by containers above it- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()];
    if (containerOnTheShipInThisPlace != nullptr) {
        errorList.emplace_back(
                "cannot move container to index occupied by other container, or space is not valid to this ship plan- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (!solidGround(shipMap, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    //move succeed
    shipMap->getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()] = containerOnTheShipInThisPlace;
    shipMap->getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;

}

int checkRejectOperation(shared_ptr<ShipMap> shipMap, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                          int maxNumberPortLoaded,
                          list<SimulatorError> &errorList, const string &currentPort) {
    int result=0;
    cargoOp.getContainer()->setIsContainerReject(1);
    //container destination is here no reason to load
    if (cargoOp.getContainer()->getDestination() == currentPort) {
        return result;
    }
    //container illegal should reject
    if (!cargoOp.getContainer()->isContainerLegal()) {
        return result;
    }
    int numIdInLoadList = 0;
    for (auto cont :loadList) {
        if (cont->getId() == cargoOp.getContainer()->getId()) {
            numIdInLoadList++;
        }
    }
    if (numIdInLoadList == 0) {
        errorList.emplace_back("rejected container with id which is not in the container list to load in this port",
                               SimErrorType::OPERATION_PORT,
                               cargoOp);
        return result;
    }
    //check container Id on ship
    auto itr = shipMap->getContainerIDOnShip().find(cargoOp.getContainer()->getId());
    if (itr == shipMap->getContainerIDOnShip().cend()) {
        //destination in route
        if (cargoOp.getContainer()->getPortIndex() > 0) {
            MapIndex index = MapIndex::firstLegalIndexPlace(shipMap);
            if (index.validIndex()) {
                errorList.emplace_back(
                        "rejected container with valid destination while there is still place on the ship",
                        SimErrorType::OPERATION_PORT, cargoOp);
                return result;
            } else {
                result|=1<<18;
                if (maxNumberPortLoaded > cargoOp.getContainer()->getPortIndex()) {
                    errorList.emplace_back(
                            "rejected container with closer destination and loaded container with a further destination instead",
                            SimErrorType::OPERATION_PORT, cargoOp);
                    return result;
                }
            }
        }
    }
    return result;
}

//make sure nothing left on port with no reason
void nothingLeftNoReason(map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToCargoOp,
                         list<SimulatorError> &errorList,
                         const string &currentPort, list<shared_ptr<Container>> &doubleIdList) {
    for (auto pair : rememberToLoadAgainIdToCargoOp) {
        bool found = false;
        for (auto itr = doubleIdList.begin(); itr != doubleIdList.end() && !found; itr++) {
            if ((*itr)->getId() == pair.first && (*itr)->getDestination() == currentPort) {
                errorList.emplace_back(
                        "when rejects double id from the container list to load in this port, it rejects the first container and choose other container to load- ",
                        SimErrorType::OPERATION_PORT, *pair.second);
                found = true;
            }
        }
        if (!found) {
            errorList.emplace_back(
                    "unload container in port- " + currentPort + " instead of destination port " +
                    pair.second->getContainer()->getDestination() +
                    " ", SimErrorType::OPERATION_PORT, *pair.second);
        }
    }
}

void checkAllContainersRejectedOrLoaded(list<shared_ptr<Container>> &loadList, list<SimulatorError> &errorList) {
    for (auto container: loadList) {
        if (container->getIsContainerLoaded() == 0 && container->getIsContainerReject() == 0) {
            errorList.emplace_back("container id- " + container->getId() + " was not rejected or loaded",
                                   SimErrorType::GENERAL_PORT);
            break;
        }
    }
}

int SimulatorAlgoCheck::checkAlgoCorrect(shared_ptr<ShipMap> shipMap, list<string>& route, WeightBalanceCalculator &calculator,
                                     list<shared_ptr<CargoOperation>> &cargoOpsList,
                                     list<shared_ptr<Container>> &loadList,
                                     const string &currentPort, int &numberLoads, int &numberUnloads,
                                     list<SimulatorError> &errorList, list<shared_ptr<Container>> &doubleIdList) {
    int result=0;
    map<string, shared_ptr<CargoOperation>> rememberToLoadAgainIdToCargoOp;
    int number = 1;
    int maxNumberPortLoaded = 0;
    orderListLoadContainer(loadList, route);
    loadList.sort(compByPortDest);
    for (auto cargoOp: cargoOpsList) {
        cargoOp->setPlaceInList(number);
        number++;
        AbstractAlgorithm::Action op = cargoOp->getOp();
        if (cargoOp->getContainer() != nullptr) {
            if (calculator.tryOperation((char) op, cargoOp->getContainer()->getWeight(),
                                        cargoOp->getIndex().getCol(),
                                        cargoOp->getIndex().getRow()) !=
                WeightBalanceCalculator::BalanceStatus::APPROVED) {
                //TODO: calculator denied operation
                errorList.emplace_back("weight calculator does not approve this operation- operation ignored",
                                       SimErrorType::OPERATION_PORT, *cargoOp);
            } else {
                switch (op) {
                    case AbstractAlgorithm::Action::LOAD:
                        numberLoads++;
                        checkLoadOperation(shipMap, *cargoOp, loadList, rememberToLoadAgainIdToCargoOp, currentPort,
                                           maxNumberPortLoaded, errorList);
                        break;
                    case AbstractAlgorithm::Action::UNLOAD:
                        numberUnloads++;
                        checkUnloadOperation(shipMap, *cargoOp, currentPort, rememberToLoadAgainIdToCargoOp, errorList);
                        break;
                    case AbstractAlgorithm::Action::MOVE:
                        checkMoveOperation(shipMap, *cargoOp, errorList);
                        break;
                    case AbstractAlgorithm::Action::REJECT:
                        break;
                    default:
                        //TODO: deal with operation which not CargoOperation - maybe in targil 2
                        break;
                }
            }
        } else {
            errorList.emplace_back(
                    "operation number" + std::to_string(cargoOp->getPlaceInList()) + "use unknown container ID",
                    SimErrorType::GENERAL_PORT);
        }
    }
    for (const auto& cargoOp: cargoOpsList) {
        if (cargoOp->getOp() == AbstractAlgorithm::Action::REJECT) {
            result|= checkRejectOperation(shipMap, *cargoOp, loadList, maxNumberPortLoaded, errorList, currentPort);
        }
    }
    nothingLeftNoReason(rememberToLoadAgainIdToCargoOp, errorList, currentPort,
                        doubleIdList); //make sure nothing left on port with no reason
    checkAllContainersRejectedOrLoaded(loadList, errorList);
    checkIfAllUnloaded(shipMap, currentPort, errorList);

    return result;
}

