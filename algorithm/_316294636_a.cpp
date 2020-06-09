#include "_316294636_a.h"
#include <map>

using std::map;
REGISTER_ALGORITHM (_316294636_a)

int _316294636_a::loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) {
    int result = 0;
    MapIndex loadIndex = MapIndex::firstLegalIndexPlaceHorizontal(this->shipMap.get());
    CargoOperation op(AbstractAlgorithm::Action::LOAD, cont, loadIndex);
    if (this->calculator.tryOperation('L', op.getContainer()->getWeight(), op.getIndex().getCol(),
                                      op.getIndex().getRow()) == WeightBalanceCalculator::BalanceStatus::APPROVED) {
        opList.push_back(op);
        this->shipMap->getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
        this->shipMap->getContainerIDOnShip().insert(cont->getId());
    } else {
        //TODO: calculator denied operation
    }
    return result;
}