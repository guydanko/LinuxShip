#include "_316294636_b.h"
#include <map>
#include "../common/FileHandler.h"

using std::map;
REGISTER_ALGORITHM (_316294636_b)


void
_316294636_b::moveTower(MapIndex index, const string &portName, list<shared_ptr<Container>> &rememberLoadAgain,
                        list<CargoOperation> &opList) {
    for (int i = this->shipMap->getHeight() - 1; i >= index.getHeight(); i--) {
        if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            //discover container should be unload again later
            if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getDestination().compare(
                    portName) != 0) {
                rememberLoadAgain.push_back(this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]);
            }
            //discover container should be unload in this port
            this->shipMap->getContainerIDOnShip().erase(
                    this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
            opList.emplace_back(AbstractAlgorithm::Action::UNLOAD,
                                this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                MapIndex(i, index.getRow(), index.getCol()));
            this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        }
    }
}
