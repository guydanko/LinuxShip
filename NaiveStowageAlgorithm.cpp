
#include "NaiveStowageAlgorithm.h"
#include <map>
#include "FileHandler.h"
#include "WeightBalanceCalculator.h"

using std::map;

void NaiveStowageAlgorithm::tryToMove(int i, MapIndex index, shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain,
                                      list<CargoOperation> &opList) {
    MapIndex moveIndex = MapIndex::isPlaceToMove(MapIndex(i, index.getRow(), index.getCol()), this->ship->getShipMap());
    //can move on the ship
    if (moveIndex.validIndex()) {
        CargoOperation opUnload(AbstractAlgorithm::Action::UNLOAD,
                                this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()));
        CargoOperation opLoad(AbstractAlgorithm::Action::LOAD,
                              this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()], index);
        if (this->calculator.tryOperation('L', opLoad.getContainer()->getWeight(), opLoad.getIndex().getCol(),
                                          opLoad.getIndex().getRow()) ==
            WeightBalanceCalculator::BalanceStatus::APPROVED &&
            this->calculator.tryOperation('U', opUnload.getContainer()->getWeight(), opUnload.getIndex().getCol(),
                                          opUnload.getIndex().getRow()) ==
            WeightBalanceCalculator::BalanceStatus::APPROVED) {
            opList.emplace_back(AbstractAlgorithm::Action::MOVE,
                                this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()), moveIndex);
            this->ship->getShipMap().getShipMapContainer()[moveIndex.getHeight()][moveIndex.getRow()][moveIndex.getCol()] = this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()];
            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        } else {
            //TODO: calculator denied operation
        }
    }
        // must unload and load again
    else {
        CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                          this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],
                          MapIndex(i, index.getRow(), index.getCol()));
        if (this->calculator.tryOperation('U', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                          op.getIndex().getRow()) == WeightBalanceCalculator::BalanceStatus::APPROVED) {
            this->ship->getShipMap().getContainerIDOnShip().erase(
                    this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
            opList.push_back(op);
            rememberLoadAgain->push_back(
                    this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]);
            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        } else {
            //TODO: calculator denied operation
        }
    }
}

void NaiveStowageAlgorithm::moveTower(MapIndex index, const string &portName,
                                      shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain,
                                      list<CargoOperation> &opList) {

    for (int i = this->ship->getShipMap().getHeight() - 1; i >= index.getHeight(); i--) {
        if (this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            //discover container should be unload here
            if (this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getDestination().compare(
                    portName) == 0) {
                CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                                  this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],
                                  MapIndex(i, index.getRow(), index.getCol()));
                if (this->calculator.tryOperation('U', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                                  op.getIndex().getRow()) ==
                    WeightBalanceCalculator::BalanceStatus::APPROVED) {
                    this->ship->getShipMap().getContainerIDOnShip().erase(
                            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
                    opList.push_back(op);
                    this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
                } else {
                    //TODO: calculator denied operation
                }
            } else {
                tryToMove(i, index, rememberLoadAgain, opList);
            }
        }
    }
}

shared_ptr<list<shared_ptr<Container>>>
NaiveStowageAlgorithm::unloadContainerByPort(const string &portName, list<CargoOperation> &opList) {
    shared_ptr<list<shared_ptr<Container>>> rememberToLoad = std::make_shared<list<shared_ptr<Container>>>();
    for (int i = 0; i < this->ship->getShipMap().getRows(); i++) {
        for (int j = 0; j < this->ship->getShipMap().getCols(); j++) {
            for (int k = 0; k < this->ship->getShipMap().getHeight(); k++) {
                if (this->ship->getShipMap().getShipMapContainer()[k][i][j] != nullptr) {
                    string name = this->ship->getShipMap().getShipMapContainer()[k][i][j]->getDestination();
                    if (this->ship->getShipMap().getShipMapContainer()[k][i][j]->getDestination().compare(portName) ==
                        0) {
                        moveTower(MapIndex(k, i, j), portName, rememberToLoad, opList);
                    }
                }
            }
        }

    }
    return rememberToLoad;
}

void NaiveStowageAlgorithm::loadAgain(shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain,
                                      list<CargoOperation> &opList) {
    for (auto cont: *rememberLoadAgain) {
        auto itr = this->ship->getShipMap().getContainerIDOnShip().find(cont->getId());
        //no id like this cont on the ship- legal cont
        if (itr == this->ship->getShipMap().getContainerIDOnShip().cend()) {
            MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
            if (loadIndex.validIndex()) {
                CargoOperation cargoOp(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
                if (this->calculator.tryOperation('L', cargoOp.getContainer()->getWeight(),
                                                  cargoOp.getIndex().getCol(),
                                                  cargoOp.getIndex().getRow()) ==
                    WeightBalanceCalculator::BalanceStatus::APPROVED) {
                    this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
                    opList.push_back(cargoOp);
                } else {
                    //TODO: calculator denied operation
                }
            }
        }
            // id is taken
        else {
            opList.emplace_front(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
        }

    }
}

void orderLoadContainer(list<shared_ptr<Container>> &containerListToLoad, list<string> &route,
                        list<CargoOperation> &opList) {
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
        if (cont->isContainerLegal()) {
            auto itr = portNumberMap.find(cont->getDestination());
            // container destination is not in route- REJECT
            if (itr == portNumberMap.end()) {
                opList.emplace_back(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
            } else {
                cont->setPortIndex(portNumberMap[itr->first]);
            }
        }
            // container has illegal parameters
        else {
            opList.emplace_back(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
        }
    }

}

void NaiveStowageAlgorithm::loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) {

    auto itr = this->ship->getShipMap().getContainerIDOnShip().find(cont->getId());
    //no id like this cont on the ship- legal cont
    if (itr == this->ship->getShipMap().getContainerIDOnShip().cend()) {
        MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
        if (loadIndex.validIndex()) {
            CargoOperation op(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
            if (this->calculator.tryOperation('L', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                              op.getIndex().getRow()) ==
                WeightBalanceCalculator::BalanceStatus::APPROVED) {
                opList.push_back(op);
                this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
                this->ship->getShipMap().getContainerIDOnShip().insert(cont->getId());
            } else {
                //TODO: calculator denied operation
            }
        }
            //no place on ship
        else {
            opList.emplace_back(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
        }
    }
        // id is taken
    else {
        opList.emplace_front(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
    }

}

bool cmp(const shared_ptr<Container> cont1, const shared_ptr<Container> cont2) {
    return cont1->getPortIndex() < cont2->getPortIndex();
}

void
NaiveStowageAlgorithm::loadNewContainers(list<shared_ptr<Container>> &containerListToLoad,
                                         list<CargoOperation> &opList,
                                         const string &currentPort) {
    orderLoadContainer(containerListToLoad, this->ship->getShipRoute(), opList);
    containerListToLoad.sort(cmp);
    for (auto cont : containerListToLoad) {

        if (cont->getPortIndex() > 0 && cont->getDestination() != currentPort) {
            loadOneContainer(cont, opList);
        }

    }
}

int NaiveStowageAlgorithm::rejectDoubleId(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) {
    map<string, int> containerMap;
    map<string, bool> containerMapFirst;
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
                opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
                itr = loadList.erase(itr);
            }
        } else {
            itr++;
        }
    }
    for (auto itr = loadList.begin(); itr != loadList.cend();) {
        auto place = this->ship->getShipMap().getContainerIDOnShip().find((*itr)->getId());
        if (place != this->ship->getShipMap().getContainerIDOnShip().cend()) {
            opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
            itr = loadList.erase(itr);
        } else {
            itr++;
        }
    }
    return 0;
}

int NaiveStowageAlgorithm::readShipPlan(const std::string &full_path_and_file_name) {
    this->ship = FileHandler::createShipFromFile(full_path_and_file_name);
    return this->ship == nullptr;
}

int NaiveStowageAlgorithm::readShipRoute(const std::string &full_path_and_file_name) {
    this->ship->setShipRoute(FileHandler::fileToRouteList(full_path_and_file_name));
    const string &portName = this->ship->getShipRoute().front();
    //always? what if cant open file?
    return 0;
}

int NaiveStowageAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator) {
    this->calculator = calculator;
    return 0;
}

int NaiveStowageAlgorithm::getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                                   const std::string &output_full_path_and_file_name) {
    if (!this->ship->getShipRoute().empty()) {
        const string portName = this->ship->getShipRoute().front();
        this->ship->getShipRoute().pop_front();
        list<shared_ptr<Container>> containerListToLoadInThisPort = FileHandler::fileToContainerList(
                input_full_path_and_file_name); //always ok? what if cant open file?
        list<CargoOperation> opList;
        rejectDoubleId(containerListToLoadInThisPort, opList);
        shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain = this->unloadContainerByPort(portName, opList);
        this->loadAgain(rememberLoadAgain, opList);
        this->loadNewContainers(containerListToLoadInThisPort, opList, portName);
        //always ok? what if cant open file?
        FileHandler::operationsToFile(opList, output_full_path_and_file_name, portName);
        return 0;
    }
    return 1;
}