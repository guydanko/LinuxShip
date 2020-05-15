#include "_316294636_a.h"
#include <map>
#include "../common/FileHandler.h"

using std::map;
REGISTER_ALGORITHM (_316294636_a)



void _316294636_a::moveTower(MapIndex index, const string &portName,
                             list<shared_ptr<Container>> &rememberLoadAgain,
                             list<CargoOperation> &opList) {

    for (int i = this->shipMap->getHeight() - 1; i >= index.getHeight(); i--) {
        if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr) {
            //discover container should be unload here
            if (this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getDestination().compare(
                    portName) == 0) {
                CargoOperation op(AbstractAlgorithm::Action::UNLOAD,
                                  this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()],
                                  MapIndex(i, index.getRow(), index.getCol()));
                if (this->calculator.tryOperation('U', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                                  op.getIndex().getRow()) ==
                    WeightBalanceCalculator::BalanceStatus::APPROVED) {
                    this->shipMap->getContainerIDOnShip().erase(
                            this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
                    opList.push_back(op);
                    this->shipMap->getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
                } else {
                    //TODO: calculator denied operation
                }
            } else {
                tryToMove(i, index, rememberLoadAgain, opList);
            }
        }
    }
}
