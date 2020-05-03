
#include "IncorrectAlgorithm.h"
#include "FileHandler.h"
#include <map>

void IncorrectAlgorithm::tryToMove(int i, MapIndex index, list<CargoOperation> &opList) {
    MapIndex moveIndex = MapIndex::isPlaceToMove(MapIndex(i, index.getRow(), index.getCol()), this->shipMap);
    //can move on the ship
    if (moveIndex.validIndex()) {
        CargoOperation opUnload(AbstractAlgorithm::Action::UNLOAD,
                                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()));
        CargoOperation opLoad(AbstractAlgorithm::Action::LOAD,
                              this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                              moveIndex);
        opList.emplace_back(AbstractAlgorithm::Action::MOVE,
                            this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                            MapIndex(i, index.getRow(), index.getCol()), moveIndex);
        this->shipMap->getShipMapContainer()[moveIndex.getHeight()][moveIndex.getRow()][moveIndex.getCol()] = this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()];
        this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
    }
        // must unload and load again
    else {
        CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                          this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                          MapIndex(i, index.getRow(), index.getCol()));
        this->shipMap->getContainerIDOnShip().erase(
                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
        opList.push_back(op);
        this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
    }
}

void IncorrectAlgorithm::moveTower(MapIndex index, const string &portName, list<CargoOperation> &opList) {
    for (int i = this->shipMap->getHeight() - 1; i >= index.getHeight(); i--) {
        if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            //discover container should be unload here
            if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getDestination().compare(
                    portName) == 0) {
                CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                                  this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                  MapIndex(i, index.getRow(), index.getCol()));
                this->shipMap->getContainerIDOnShip().erase(
                        this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
                opList.push_back(op);
                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
            } else {
                tryToMove(i, index, opList);
            }
        }
    }
}


void IncorrectAlgorithm::unloadContainerByPort(const string &portName, list<CargoOperation> &opList) {
    for (int i = 0; i < this->shipMap->getRows(); i++) {
        for (int j = 0; j < this->shipMap->getCols(); j++) {
            for (int k = 0; k < this->shipMap->getHeight(); k++) {
                if (this->shipMap->getShipMapContainer()[k][i][j] != nullptr)
                    if (this->shipMap->getShipMapContainer()[k][i][j]->getDestination().compare(portName) ==
                        0) {
                        moveTower(MapIndex(k, i, j), portName, opList);
                    }
            }
        }
    }

}

void IncorrectAlgorithm::loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) {
    MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->shipMap);
    if (loadIndex.validIndex()) {
        CargoOperation op(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
        opList.push_back(op);
        this->shipMap->getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
        this->shipMap->getContainerIDOnShip().insert(cont->getId());
    }
        //no place on ship
    else {
        opList.emplace_back(AbstractAlgorithm::Action::REJECT, cont, MapIndex());
    }
}

void
IncorrectAlgorithm::loadNewContainers(list<shared_ptr<Container>> &containerListToLoad, list<CargoOperation> &opList,
                                      const string &currentPort) {
    for (auto cont : containerListToLoad) {
        //unnecessary if - only for use currentPort
        if (currentPort.length() > 0) {
            loadOneContainer(cont, opList);
        }
    }
}

int IncorrectAlgorithm::readShipPlan(const std::string &full_path_and_file_name) {
    auto shipPtr = std::make_shared<shared_ptr<ShipMap>>(std::make_shared<ShipMap>());
    int result = FileHandler::createShipMapFromFile(full_path_and_file_name, shipPtr);
    this->shipMap = *shipPtr;
    return result;
}

int IncorrectAlgorithm::readShipRoute(const std::string &full_path_and_file_name) {
    return FileHandler::fileToRouteList(full_path_and_file_name, this->route);
}

int IncorrectAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator &calculator) {
    this->calculator = calculator;
    return 0;
}

int IncorrectAlgorithm::getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                                const std::string &output_full_path_and_file_name) {
    int result = 0;
    const string &portName = this->route.front();
    this->route.pop_front();
    list<shared_ptr<Container>> containerListToLoadInThisPort = {};
    result += FileHandler::fileToContainerList(input_full_path_and_file_name, containerListToLoadInThisPort);
    list<CargoOperation> opList;
    this->unloadContainerByPort(portName, opList);
    this->loadNewContainers(containerListToLoadInThisPort, opList, portName);
    FileHandler::operationsToFile(opList, output_full_path_and_file_name, portName);
    return result;
}