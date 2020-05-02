#include "SimulatorAlgoCheck.h"
#include <map>

using std::map;

bool indexInLimit(shared_ptr<Ship> ship, MapIndex index) {
    if (index.getHeight() < 0 || index.getHeight() >= ship->getShipMap().getHeight()) {
        return false;
    }
    if (index.getRow() < 0 || index.getRow() >= ship->getShipMap().getRows()) {
        return false;
    }
    return !(index.getCol() < 0 || index.getCol() >= ship->getShipMap().getCols());
}

bool containerAbove(shared_ptr<Ship> ship, MapIndex index) {
    for (int i = ship->getShipMap().getHeight() - 1; i > index.getHeight(); i--) {
        if (ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            return true;
        }
    }
    return false;
}

void indexAccessible(shared_ptr<Ship> ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    if (!indexInLimit(ship, cargoOp.getIndex())) {
        errorList.emplace_back("illegal index, exceeds ship plan limits- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot reach container in index, blocked above by containers- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
}

bool solidGround(shared_ptr<Ship> ship, MapIndex index) {
    if (index.getHeight() == 0) {
        return true;
    }
    int i = index.getHeight() - 1;
    return ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr;
}

void checkIfAllUnloaded(shared_ptr<Ship> ship, const string &port, list<SimulatorError> &errorList) {
    for (int i = 0; i < ship->getShipMap().getHeight(); i++) {
        for (int j = 0; j < ship->getShipMap().getRows(); j++) {
            for (int k = 0; k < ship->getShipMap().getCols(); k++) {
                if (ship->getShipMap().getShipMapContainer()[i][j][k] != nullptr &&
                    ship->getShipMap().getShipMapContainer()[i][j][k] != ship->getShipMap().getImaginary()) {
                    if (ship->getShipMap().getShipMapContainer()[i][j][k]->getDestination() == port) {
                        errorList.emplace_back("Container on the ship with id- " +
                                               ship->getShipMap().getShipMapContainer()[i][j][k]->getId() +
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

void checkLoadOperation(shared_ptr<Ship> ship, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                        map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToIndex,
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
    shared_ptr<Container> containerFromList = nullptr;
    indexAccessible(ship, cargoOp, errorList);
    if (ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] !=
        nullptr) {
        errorList.emplace_back("index is occupied by container or cannot be place due to ship plan- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    if (!solidGround(ship, cargoOp.getIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        cargoOp.getContainer()->setIsContainerLoaded(1);
        return;
    }
    for (auto pair: rememberToLoadAgainIdToIndex) {
        if (pair.first == cargoOp.getContainer()->getId()) {
            numberOfIdInList++;
        }
    }
    for (auto container: loadList) {
        if (container->getId() == cargoOp.getContainer()->getId() && container->getIsContainerLoaded() != 1) {
            numberOfIdInList++;
            containerFromList = container;
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
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = cargoOp.getContainer();
    cargoOp.getContainer()->setIsContainerLoaded(1);
    //load from remember
    if (containerFromList == nullptr) {
        ship->getShipMap().getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        auto itrRemember = rememberToLoadAgainIdToIndex.find(cargoOp.getContainer()->getId());
        if (itrRemember != rememberToLoadAgainIdToIndex.cend()) {
            rememberToLoadAgainIdToIndex.erase(itrRemember);
        }
    }
        //load from loadList
    else {
        auto itr = ship->getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
        if (itr != ship->getShipMap().getContainerIDOnShip().cend()) {
            errorList.emplace_back("should not load container because this id is already on the ship",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        ship->getShipMap().getContainerIDOnShip().insert(cargoOp.getContainer()->getId());
        if (cargoOp.getContainer()->getDestination() == currentPort) {
            errorList.emplace_back("should not load container with current port destination",
                                   SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() == 0) {
            errorList.emplace_back("should not load container with destination-" + containerFromList->getDestination() +
                                   " which is not in the ship's route", SimErrorType::OPERATION_PORT, cargoOp);
            return;
        }
        if (containerFromList->getPortIndex() > maxNumberPortLoaded) {
            maxNumberPortLoaded = containerFromList->getPortIndex();
        }
    }
}

void checkUnloadOperation(shared_ptr<Ship> ship, CargoOperation &cargoOp, const string &currentPort,
                          map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToIndex,
                          list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == ship->getShipMap().getImaginary()) {
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
    ship->getShipMap().getContainerIDOnShip().erase(
            ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()]->getId());
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;
    //maybe future error
    if (cargoOp.getContainer()->getDestination().compare(currentPort) != 0) {
        cargoOp.getContainer()->setIsContainerLoaded(0);
        rememberToLoadAgainIdToIndex[cargoOp.getContainer()->getId()] = std::make_shared<CargoOperation>(cargoOp);
    }
}

void checkMoveOperation(shared_ptr<Ship> ship, CargoOperation &cargoOp, list<SimulatorError> &errorList) {
    indexAccessible(ship, cargoOp, errorList);
    shared_ptr<Container> containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    if (containerOnTheShipInThisPlace == nullptr ||
        containerOnTheShipInThisPlace == ship->getShipMap().getImaginary()) {
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
    if (!indexInLimit(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("index to move is not in ship plan- operation ignored", SimErrorType::OPERATION_PORT,
                               cargoOp);
        return;
    }
    if (containerAbove(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot reach move index, it is blocked by containers above it- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()];
    if (containerOnTheShipInThisPlace != nullptr) {
        errorList.emplace_back(
                "cannot move container to index occupied by other container, or space is not valid to this ship plan- operation ignored",
                SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    if (!solidGround(ship, cargoOp.getMoveIndex())) {
        errorList.emplace_back("cannot place container without underneath platform- operation ignored",
                               SimErrorType::OPERATION_PORT, cargoOp);
        return;
    }
    containerOnTheShipInThisPlace = ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()];
    //move succeed
    ship->getShipMap().getShipMapContainer()[cargoOp.getMoveIndex().getHeight()][cargoOp.getMoveIndex().getRow()][cargoOp.getMoveIndex().getCol()] = containerOnTheShipInThisPlace;
    ship->getShipMap().getShipMapContainer()[cargoOp.getIndex().getHeight()][cargoOp.getIndex().getRow()][cargoOp.getIndex().getCol()] = nullptr;

}

void checkRejectOperation(shared_ptr<Ship> ship, CargoOperation &cargoOp, list<shared_ptr<Container>> &loadList,
                          int maxNumberPortLoaded,
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
    for (auto cont :loadList) {
        if (cont->getId() == cargoOp.getContainer()->getId()) {
            numIdInLoadList++;
        }
    }
    if (numIdInLoadList == 0) {
        errorList.emplace_back("rejected container with id which is not in the container list to load in this port",
                               SimErrorType::OPERATION_PORT,
                               cargoOp);
        return;
    }
    //check container Id on ship
    auto itr = ship->getShipMap().getContainerIDOnShip().find(cargoOp.getContainer()->getId());
    if (itr == ship->getShipMap().getContainerIDOnShip().cend()) {
        //destination in route
        if (cargoOp.getContainer()->getPortIndex() > 0) {
            MapIndex index = MapIndex::firstLegalIndexPlace(ship->getShipMap());
            if (index.validIndex()) {
                errorList.emplace_back(
                        "rejected container with valid destination while there is still place on the ship",
                        SimErrorType::OPERATION_PORT, cargoOp);
                return;
            } else {
                if (maxNumberPortLoaded > cargoOp.getContainer()->getPortIndex()) {
                    errorList.emplace_back(
                            "rejected container with closer destination and loaded container with a further destination instead",
                            SimErrorType::OPERATION_PORT, cargoOp);
                    return;
                }
            }
        }
    }
}

//make sure nothing left on port with no reason
void nothingLeftNoReason(map<string, shared_ptr<CargoOperation>> &rememberToLoadAgainIdToIndex,
                         list<SimulatorError> &errorList,
                         const string &currentPort) {
    for (auto pair : rememberToLoadAgainIdToIndex) {
        errorList.emplace_back(
                "unload container in port- " + currentPort + " instead of destination port " +
                pair.second->getContainer()->getDestination() +
                " ", SimErrorType::OPERATION_PORT, *pair.second);
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

list<SimulatorError>
SimulatorAlgoCheck::checkAlgoCorrect(shared_ptr<Ship> ship,WeightBalanceCalculator& calculator, list<shared_ptr<CargoOperation>> &cargoOpsList,
                            list<shared_ptr<Container>> &loadList,
                            const string &currentPort, int &numberLoads, int &numberUnloads,
                            list<SimulatorError> &errorList) {
    map<string, shared_ptr<CargoOperation>> rememberToLoadAgainIdToIndex;
    int number = 1;
    int maxNumberPortLoaded = 0;
    orderListLoadContainer(loadList, ship->getShipRoute());
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
                        checkLoadOperation(ship, *cargoOp, loadList, rememberToLoadAgainIdToIndex, currentPort,
                                           maxNumberPortLoaded, errorList);
                        break;
                    case AbstractAlgorithm::Action::UNLOAD:
                        numberUnloads++;
                        checkUnloadOperation(ship, *cargoOp, currentPort, rememberToLoadAgainIdToIndex, errorList);
                        break;
                    case AbstractAlgorithm::Action::MOVE:
                        checkMoveOperation(ship, *cargoOp, errorList);
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
    for (auto cargoOp: cargoOpsList) {
        if (cargoOp->getOp() == AbstractAlgorithm::Action::REJECT) {
            checkRejectOperation(ship, *cargoOp, loadList, maxNumberPortLoaded, errorList, currentPort);
        }
    }
    nothingLeftNoReason(rememberToLoadAgainIdToIndex, errorList,
                        currentPort); //make sure nothing left on port with no reason
    checkAllContainersRejectedOrLoaded(loadList, errorList);
    checkIfAllUnloaded(ship, currentPort, errorList);
    return errorList;
}

