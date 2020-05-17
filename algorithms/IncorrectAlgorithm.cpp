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
                itr = loadList.erase(itr);
            }
            findSecond[(*itr)->getId()] +=1;
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
    IncorrectAlgorithm::rejectIllagalContainer(loadList, opList);
    if(currentPort==""){
        result=2;
    }
    else{
        result=0;
    }
//    result |=this->rejectDestNotInRoute(loadList, opList, currentPort);
    this->rejectDoubleId(loadList, opList);
    return result;
}
void
IncorrectAlgorithm::moveTower(MapIndex index, const string &portName, list<shared_ptr<Container>> &rememberLoadAgain,
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
void IncorrectAlgorithm::loadAgain(list<shared_ptr<Container>>& rememberLoadAgain, list<CargoOperation> &opList) {
    if(rememberLoadAgain.size()>2){
        opList.front();
    } else{
        opList.back();
    }
}
