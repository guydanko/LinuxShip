
#include "Ship.h"
#include "CargoOperation.h"
#include "WeightBalanceCalculator.h"

#ifndef SHIPGIT_ABSRACTSTOWAGEALGORITHM_H
#define SHIPGIT_ABSRACTSTOWAGEALGORITHM_H
using std::string;

class AbstractStowageAlgorithm {

protected:
    Ship* ship= nullptr;
    WeightBalanceCalculator* calculator= nullptr;
public:
    AbstractStowageAlgorithm(Ship* ship= nullptr, WeightBalanceCalculator* calculator= nullptr ){this->ship=ship; this->calculator=calculator;}
    virtual list<CargoOperation> getInstructionsForCargo(list<Container*> containerListToLoad,const string& portName) =0;
    virtual void loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList )=0; //load container which unload in order to let other container unload
    virtual void loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort)=0;
    virtual  list<Container*>* unloadContainerByPort(const string& portName, list<CargoOperation>& opList )=0; //unload container with destination portName
    void setShip(Ship* ship){this->ship=ship;}
    virtual string getName() const=0;
    //prepare option to work with calculator
    WeightBalanceCalculator* getCalculator(){return this->calculator;}
    void setCalculator( WeightBalanceCalculator* calc){this->calculator=calc;}
    virtual ~AbstractStowageAlgorithm(){};

};
#endif //SHIPGIT_ABSRACTSTOWAGEALGORITHM_H
