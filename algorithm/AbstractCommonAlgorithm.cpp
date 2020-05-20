#include "AbstractCommonAlgorithm.h"

void AbstractCommonAlgorithm::tryToMove(int i, MapIndex index, list<shared_ptr<Container>> &rememberLoadAgain,
                                        list<CargoOperation> &opList) {
    MapIndex moveIndex = MapIndex::isPlaceToMove(MapIndex(i, index.getRow(), index.getCol()), this->shipMap.get());
    //can move on the ship
    if (moveIndex.validIndex()) {
        CargoOperation opUnload(AbstractAlgorithm::Action::UNLOAD,
                                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()));
        CargoOperation opLoad(AbstractAlgorithm::Action::LOAD,
                              this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()], index);
        if (this->calculator.tryOperation('L', opLoad.getContainer()->getWeight(), opLoad.getIndex().getCol(),
                                          opLoad.getIndex().getRow()) ==
            WeightBalanceCalculator::BalanceStatus::APPROVED &&
            this->calculator.tryOperation('U', opUnload.getContainer()->getWeight(), opUnload.getIndex().getCol(),
                                          opUnload.getIndex().getRow()) ==
            WeightBalanceCalculator::BalanceStatus::APPROVED) {
            opList.emplace_back(AbstractAlgorithm::Action::MOVE,
                                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()), moveIndex);
            this->shipMap->getShipMapContainer()[moveIndex.getHeight()][moveIndex.getRow()][moveIndex.getCol()] = this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()];
            this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        } else {
            //TODO: calculator denied operation
        }
    }
        // must unload and load again
    else {
        CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                          this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                          MapIndex(i, index.getRow(), index.getCol()));
        if (this->calculator.tryOperation('U', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                          op.getIndex().getRow()) == WeightBalanceCalculator::BalanceStatus::APPROVED) {
            this->shipMap->getContainerIDOnShip().erase(
                    this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
            opList.push_back(op);
            rememberLoadAgain.push_back(
                    this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]);
            this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        } else {
            //TODO: calculator denied operation
        }
    }
}

void AbstractCommonAlgorithm::unloadContainerByPort(const string &portName, list<CargoOperation> &opList,
                                                    list<shared_ptr<Container>> &rememberLoadAgain) {

    for (int i = 0; i < this->shipMap->getRows(); i++) {
        for (int j = 0; j < this->shipMap->getCols(); j++) {
            for (int k = 0; k < this->shipMap->getHeight(); k++) {
                if (this->shipMap->getShipMapContainer()[k][i][j] != nullptr) {
                    string name = this->shipMap->getShipMapContainer()[k][i][j]->getDestination();
                    if (this->shipMap->getShipMapContainer()[k][i][j]->getDestination() == portName) {
                        moveTower(MapIndex(k, i, j), portName, rememberLoadAgain, opList);
                    }
                }
            }
        }
    }
}

void AbstractCommonAlgorithm::loadAgain(list<shared_ptr<Container>> &rememberLoadAgain,
                                        list<CargoOperation> &opList) {
    for (const auto &cont: rememberLoadAgain) {
        MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->shipMap.get());
        //should always be true because it load again container which have been on ship
        if (loadIndex.validIndex()) {
            CargoOperation cargoOp(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
            if (this->calculator.tryOperation('L', cargoOp.getContainer()->getWeight(),
                                              cargoOp.getIndex().getCol(),
                                              cargoOp.getIndex().getRow()) ==
                WeightBalanceCalculator::BalanceStatus::APPROVED) {
                this->shipMap->getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
                opList.push_back(cargoOp);
            } else {
                //TODO: calculator denied operation
            }
        }
    }
}


int AbstractCommonAlgorithm::loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) {
    int result = 0;
    MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->shipMap.get());
    if (loadIndex.validIndex()) {
        CargoOperation op(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
        if (this->calculator.tryOperation('L', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                          op.getIndex().getRow()) == WeightBalanceCalculator::BalanceStatus::APPROVED) {
            opList.push_back(op);
            this->shipMap->getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
            this->shipMap->getContainerIDOnShip().insert(cont->getId());
        } else {
            //TODO: calculator denied operation
        }
    }
        //no place on ship
    else {
        result |= 1 << 18;
        opList.emplace_back(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
    }

    return result;
}

int AbstractCommonAlgorithm::loadNewContainers(list<shared_ptr<Container>> &containerListToLoad,
                                               list<CargoOperation> &opList) {
    int result = 0;
    for (const auto &cont : containerListToLoad) {
        result |= loadOneContainer(cont, opList);
    }
    return result;
}

int AbstractCommonAlgorithm::rejectDestNotInRoute(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList,
                                                  const string &currentPort) {
    map<string, int> portNumberMap;
    int number = 1;
    int result = 0;
    for (const string &port : this->route) {
        auto itr = portNumberMap.find(port);
        if (itr == portNumberMap.end()) {
            portNumberMap[port] = number;
            number++;
        }
    }
    for (auto itr = loadList.begin(); itr != loadList.end();) {
        auto itrFind = portNumberMap.find((*itr)->getDestination());
        // container destination is not in route or container destination is current port- REJECT
        if (itrFind == portNumberMap.end() || (*itr)->getDestination() == currentPort) {
            opList.emplace_back(AbstractAlgorithm::Action::REJECT, (*itr), MapIndex());
            itr = loadList.erase(itr);
            result = 1 << 13;
        } else {
            (*itr)->setPortIndex(portNumberMap[itrFind->first]);
            itr++;
        }
    }
    return result;
}

void
AbstractCommonAlgorithm::rejectIllegalContainer(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) {
    for (auto itr = loadList.begin(); itr != loadList.end();) {
        if (!(*itr)->isContainerLegal()) {
            opList.emplace_back(Action::REJECT, (*itr));
            itr = loadList.erase(itr);
        } else {
            itr++;
        }
    }
}

int AbstractCommonAlgorithm::rejectDoubleId(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) {
    map<string, int> containerMap;
    map<string, bool> containerMapFirst;
    int result = 0;
    /*create map of id to container in load list*/
    for (const auto &cont: loadList) {
        containerMap[cont->getId()] = containerMap[cont->getId()] + 1;
        containerMapFirst[cont->getId()] = true;
    }

    for (auto itr = loadList.begin(); itr != loadList.cend();) {
        if (containerMap[(*itr)->getId()] > 1) {
            if (containerMapFirst[(*itr)->getId()]) {
                containerMapFirst[(*itr)->getId()] = false;
                itr++;
            } else {
                result |= 1 << 10;
                opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
                itr = loadList.erase(itr);
            }
        } else {
            itr++;
        }
    }
    for (auto itr = loadList.begin(); itr != loadList.cend();) {
        auto place = this->shipMap->getContainerIDOnShip().find((*itr)->getId());
        if (place != this->shipMap->getContainerIDOnShip().cend()) {
            result |= 1 << 11;
            opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
            itr = loadList.erase(itr);
        } else {
            itr++;
        }
    }
    return result;
}

int
AbstractCommonAlgorithm::rejectAllBesideShipFull(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList,
                                                 const string &currentPort) {
    int result = 0;
    this->rejectIllegalContainer(loadList, opList);
    result |= this->rejectDestNotInRoute(loadList, opList, currentPort);
    result |= this->rejectDoubleId(loadList, opList);
    return result;
}

int AbstractCommonAlgorithm::readShipPlan(const std::string &full_path_and_file_name) {
    auto shipPtr = std::make_shared<shared_ptr<ShipMap>>(std::make_shared<ShipMap>());
    int result = FileHandler::createShipMapFromFile(full_path_and_file_name, shipPtr);
    this->shipMap = *shipPtr;
    isReadShipPlanSucceed = !(result & (1 << 3) || result & (1 << 4));
    errorCode |= result;
    return result;
}

int AbstractCommonAlgorithm::readShipRoute(const std::string &full_path_and_file_name) {
    int result = FileHandler::fileToRouteList(full_path_and_file_name, this->route);
    isReadShipRouteSucceed = !(result & (1 << 7) || result & (1 << 8));
    errorCode |= result;
    return result;
}

int AbstractCommonAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator) {
    this->calculator = calculator;
    isSetCalculatorSucceed = true;
    return 0;
}

int AbstractCommonAlgorithm::getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                                     const std::string &output_full_path_and_file_name) {
    int result = 0;
    list<CargoOperation> opList = {};
    list<shared_ptr<Container>> loadList = {};
    list<shared_ptr<Container>> rememberLoadAgain = {};
    if (!isSetCalculatorSucceed || !isReadShipRouteSucceed || !isReadShipPlanSucceed) {
        FileHandler::operationsToFile(opList, output_full_path_and_file_name);
        return errorCode;
    }
    if (!this->route.empty()) {
        const string currentPort = this->route.front();
        this->route.pop_front();
        int fileResult = FileHandler::fileToContainerList(input_full_path_and_file_name, loadList);
        /* current port is the last one in route*/
        if (this->route.empty()) {
            if (!loadList.empty() || (fileResult != (1 << 16) && fileResult != 0)) {
                result = 1 << 17;
                loadList = {};
            }
        } else {
            result |= fileResult;
        }
        result |= rejectAllBesideShipFull(loadList, opList, currentPort);
        loadList.sort([](const shared_ptr<Container> cont1, const shared_ptr<Container> cont2) -> bool {
            return cont1->getPortIndex() < cont2->getPortIndex();
        });
        this->unloadContainerByPort(currentPort, opList, rememberLoadAgain);
        this->loadAgain(rememberLoadAgain, opList);
        result |= this->loadNewContainers(loadList, opList);
        FileHandler::operationsToFile(opList, output_full_path_and_file_name);
        return result;
    }
    return result;
}