
#include "NaiveStowageAlgorithm.h"
#include <map>
using std::map;
void NaiveStowageAlgorithm::tryToMove(int i,MapIndex index ,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList){
    MapIndex moveIndex = MapIndex::isPlaceToMove(MapIndex(i,index.getRow(),index.getCol()),this->ship->getShipMap());
    //can move on the ship
    if(moveIndex.validIndex()){
        CargoOperation opUnload(Operation ::UNLOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],MapIndex(i,index.getRow(),index.getCol()));
        CargoOperation opLoad(Operation ::LOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],index);
        if(this->calculator->tryOperation()==BalanceStatus::APPROVED && this->calculator->tryOperation()==BalanceStatus::APPROVED) {
            opList.emplace_back(Operation::MOVE,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],MapIndex(i, index.getRow(), index.getCol()),moveIndex);
            this->ship->getShipMap().getShipMapContainer()[moveIndex.getHeight()][moveIndex.getRow()][moveIndex.getCol()] = this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()];
            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        }
        else{
            //TODO: calculator denied operation
        }
    }
        // must unload and load again
    else{
        CargoOperation op(Operation ::UNLOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],MapIndex(i,index.getRow(),index.getCol()));
        if(this->calculator->tryOperation()==BalanceStatus::APPROVED){
            this->ship->getShipMap().getContainerIDOnShip().erase(this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
            opList.push_back(op);
            rememberLoadAgain->push_back(this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]);
            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        }
        else{
            //TODO: calculator denied operation
        }
    }
}
void NaiveStowageAlgorithm::moveTower(MapIndex index , const string& portName,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList){

    for (int i = this->ship->getShipMap().getHeight()-1; i >= index.getHeight(); i--) {
        if(this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] != nullptr){
            //discover container should be unload here
            if(this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getDestination().compare(portName) == 0) {
                CargoOperation op(Operation ::UNLOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],MapIndex(i,index.getRow(),index.getCol()));
                if(this->calculator->tryOperation()==BalanceStatus::APPROVED){
                    this->ship->getShipMap().getContainerIDOnShip().erase(this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()]->getId());
                    opList.push_back(op);
                    this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
                }
                else{
                    //TODO: calculator denied operation
                }
            }
            else{
              tryToMove(i,index,rememberLoadAgain,opList);
            }
        }
    }
}

list<Container*>* NaiveStowageAlgorithm::unloadContainerByPort(const string& portName, list<CargoOperation>& opList ){
    list<Container*>* rememberToLoad= new list<Container*>();
    for(int i = 0; i < this->ship->getShipMap().getRows(); i++ ){
        for(int j = 0; j < this->ship->getShipMap().getCols(); j++){
            for(int k = 0; k< this->ship->getShipMap().getHeight(); k++){
                if(this->ship->getShipMap().getShipMapContainer()[k][i][j] != nullptr)
                    if(this->ship->getShipMap().getShipMapContainer()[k][i][j]->getDestination().compare(portName) == 0){
                        moveTower(MapIndex(k,i,j),portName,rememberToLoad,opList);
                    }
            }
        }
    }
    return rememberToLoad;
}
void NaiveStowageAlgorithm::loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList ){
    for(Container* cont: *rememberLoadAgain){
        auto itr =  this->ship->getShipMap().getContainerIDOnShip().find(cont->getId());
        //no id like this cont on the ship- legal cont
        if(itr==  this->ship->getShipMap().getContainerIDOnShip().cend()){
            MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
            if(loadIndex.validIndex()){
                CargoOperation cargoOp(Operation ::LOAD,cont,loadIndex);
                if(this->calculator->tryOperation()==BalanceStatus::APPROVED){
                    this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()]=cont;
                    opList.push_back(cargoOp);
                }
                else{
                    //TODO: calculator denied operation
                }
            }
        }
        // id is taken
        else{
            opList.emplace_back(Operation ::REJECT,cont,MapIndex());
        }

    }
}

void orderLoadContainer(list<Container*>& containerListToLoad, list<string>& route, list<CargoOperation>& opList){
    map<string,int> portNumberMap;
    int number=1;
    for(const string& port : route ){
        auto itr = portNumberMap.find(port);
        if(itr == portNumberMap.end()){
            portNumberMap[port] = number;
            number++;
        }
    }
    for(Container* cont : containerListToLoad){
        if(cont->isContainerLegal()) {
            auto itr = portNumberMap.find(cont->getDestination());
            // container destination is not in route- REJECT
            if (itr == portNumberMap.end()) {
                opList.emplace_back(Operation::REJECT, cont,MapIndex());
            } else {
                cont->setPortIndex(portNumberMap[itr->first]);
            }
        }
            // container is illegal parameters
        else{
            opList.emplace_back(Operation::REJECT, cont,MapIndex());
        }
    }

}
void NaiveStowageAlgorithm::loadOneContainer(Container* cont, list<CargoOperation>& opList){

    auto itr =  this->ship->getShipMap().getContainerIDOnShip().find(cont->getId());
    //no id like this cont on the ship- legal cont
    if(itr==  this->ship->getShipMap().getContainerIDOnShip().cend()){
        MapIndex loadIndex= MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
        if(loadIndex.validIndex()){
            CargoOperation op(Operation ::LOAD,cont,loadIndex);
            if(this->calculator->tryOperation()==BalanceStatus::APPROVED){
                opList.push_back(op);
                this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()]=cont;
                this->ship->getShipMap().getContainerIDOnShip().insert(cont->getId());
            }
            else{
                //TODO: calculator denied operation
            }
        }
            //no place on ship
        else{
            opList.emplace_back(Operation ::REJECT,cont,MapIndex());
        }
    }
    // id is taken
    else{
        opList.emplace_back(Operation::REJECT, cont,MapIndex());
    }

}
bool cmp(const Container* cont1, const Container* cont2){
    return cont1->getPortIndex() < cont2->getPortIndex();
}
void NaiveStowageAlgorithm::loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort){
    orderLoadContainer(containerListToLoad, this->ship->getShipRoute(),opList);
    containerListToLoad.sort(cmp);
    for(Container* cont : containerListToLoad){
        if(cont->getPortIndex() > 0 && cont->getDestination()!= currentPort){
            loadOneContainer(cont,opList);
        }
        else{
            // destination not in route
            if(cont->getDestination() == currentPort){
                opList.emplace_back(Operation::REJECT, cont,MapIndex());
            }
        }
    }
}
list<CargoOperation> NaiveStowageAlgorithm::getInstructionsForCargo(list<Container*> containerListToLoadInThisPort, const string& portName) {

    list<CargoOperation> opList;
    list<Container*>* rememberLoadAgain =this->unloadContainerByPort(portName, opList);
    this->loadAgain(rememberLoadAgain, opList);
    delete rememberLoadAgain;
    this->loadNewContainers(containerListToLoadInThisPort,opList, portName);
    return opList;

}