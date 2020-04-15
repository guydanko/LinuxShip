
#include "IncorrectAlgorithm.h"
#include <map>

void IncorrectAlgorithm::tryToMove(int i,MapIndex index , list<CargoOperation>& opList){
    MapIndex moveIndex = MapIndex::isPlaceToMove(MapIndex(i,index.getRow(),index.getCol()),this->ship->getShipMap());
    //can move on the ship
    if(moveIndex.validIndex()){
        CargoOperation opUnload(Operation ::UNLOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],MapIndex(i,index.getRow(),index.getCol()));
        CargoOperation opLoad(Operation ::LOAD,this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()],moveIndex);
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
            this->ship->getShipMap().getShipMapContainer()[i][index.getRow()][index.getCol()] = nullptr;
        }
        else{
            //TODO: calculator denied operation
        }
    }
}
void IncorrectAlgorithm::moveTower(MapIndex index , const string& portName, list<CargoOperation>& opList){

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
                tryToMove(i,index,opList);
            }
        }
    }
}


list<Container*>* IncorrectAlgorithm::unloadContainerByPort(const string& portName, list<CargoOperation>& opList ){
    for(int i = 0; i < this->ship->getShipMap().getRows(); i++ ){
        for(int j = 0; j < this->ship->getShipMap().getCols(); j++){
            for(int k = 0; k< this->ship->getShipMap().getHeight(); k++){
                if(this->ship->getShipMap().getShipMapContainer()[k][i][j] != nullptr)
                    if(this->ship->getShipMap().getShipMapContainer()[k][i][j]->getDestination().compare(portName) == 0){
                        moveTower(MapIndex(k,i,j),portName,opList);
                    }
            }
        }
    }
    return nullptr;
}
void IncorrectAlgorithm::loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList ){
    if(rememberLoadAgain == nullptr){
        return;
    }
    for(Container* cont: *rememberLoadAgain){
        auto itr =  this->ship->getShipMap().getContainerIDOnShip().find(cont->getId());
        //no id like this cont on the ship- legal cont
        if(itr==  this->ship->getShipMap().getContainerIDOnShip().cend()){
            MapIndex loadIndex = MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
            if(loadIndex.validIndex()){
                CargoOperation op(Operation ::LOAD,cont,loadIndex);
                if(this->calculator->tryOperation()==BalanceStatus::APPROVED){
                    this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()]=cont;
                    opList.push_back(op);
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

void IncorrectAlgorithm::loadOneContainer(Container* cont, list<CargoOperation>& opList){
   MapIndex loadIndex= MapIndex::firstLegalIndexPlace(this->ship->getShipMap());
   if(loadIndex.validIndex()){
       CargoOperation op(Operation ::LOAD,cont,loadIndex);
       if(this->calculator->tryOperation()==BalanceStatus::APPROVED) {
           opList.push_back(op);
           this->ship->getShipMap().getShipMapContainer()[loadIndex.getHeight()][loadIndex.getRow()][loadIndex.getCol()] = cont;
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

void IncorrectAlgorithm::loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort){
    for(Container* cont : containerListToLoad ){
        //unnecessary if - only for use currentPort
        if(currentPort.length()>0){
            loadOneContainer(cont,opList);
        }
    }
}
list<CargoOperation> IncorrectAlgorithm::getInstructionsForCargo(list<Container*> containerListToLoadInThisPort, const string& portName) {
    list<CargoOperation> opList;
    this->unloadContainerByPort(portName, opList);
    this->loadNewContainers(containerListToLoadInThisPort,opList, portName);
    return opList;

}