#include "IncorrectAlgorithm.h"
#include <map>
#include "../common/FileHandler.h"

using std::map;
REGISTER_ALGORITHM (IncorrectAlgorithm)


int IncorrectAlgorithm::rejectDoubleId(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList) {
    map<string, int> containerMap;
    map<string, int> findSecond;
    /*create map of id to container in load list*/
    for (const auto &cont: loadList) {
        containerMap[cont->getId()] = containerMap[cont->getId()] + 1;
        findSecond[cont->getId()]=1;
    }
    //keep the second container with the same ID
    for (auto itr = loadList.begin(); itr != loadList.cend();) {
        if (containerMap[(*itr)->getId()] > 1) {
            if (findSecond[(*itr)->getId()]==2) {
                itr++;
            } else {
                opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
                findSecond[(*itr)->getId()]++;
                itr = loadList.erase(itr);
            }
        } else {
            itr++;
        }
    }
    for (auto itr = loadList.begin(); itr != loadList.cend();) {
        auto place = this->shipMap->getContainerIDOnShip().find((*itr)->getId());
        if (place != this->shipMap->getContainerIDOnShip().cend()) {
            opList.emplace_front(AbstractAlgorithm::Action::REJECT, *itr, MapIndex());
            itr = loadList.erase(itr);
        } else {
            itr++;
        }
    }
    return 0;
}
int
IncorrectAlgorithm::rejectAllBesideShipFull(list<shared_ptr<Container>> &loadList, list<CargoOperation> &opList,
                                            const string& currentPort) {
    int result=0;
    IncorrectAlgorithm::rejectIllegalContainer(loadList, opList);
    //needed for compilation use this parameter for something
    if(currentPort.empty()){
        result=2;
    }
    else{
        result=0;
    }
    this->rejectDoubleId(loadList, opList);
    return result;
}
int IncorrectAlgorithm::loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) {
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
