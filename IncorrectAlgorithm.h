//
// this algorithm forget container on the port
// load container while id is already taken
// load container with destination not on route
// don't check balance with calculator
// load in arbitrary order on port destination
//

#include "AbstractStowageAlgorithm.h"
#include "WeightBalanceCalculator.h"

#ifndef SHIPGIT_UNCORRECTALGORITHM_H
#define SHIPGIT_UNCORRECTALGORITHM_H

using std::string ;

class IncorrectAlgorithm: public AbstractStowageAlgorithm {

public:
    IncorrectAlgorithm(): AbstractStowageAlgorithm() {}
    IncorrectAlgorithm(Ship* ship,WeightBalanceCalculator* calculator): AbstractStowageAlgorithm(ship,calculator){}

    list<CargoOperation> getInstructionsForCargo(list<Container*> containerListToLoad,const string& portName) override ;
    void loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList ) override;
    void loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort) override;
    list<Container*>* unloadContainerByPort(const string& portName, list<CargoOperation>& opList ) override;
    virtual void moveTower(MapIndex index , const string& portName,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);
    void loadOneContainer(Container* cont, list<CargoOperation>& opList);
    string getName() const override { return "Incorrect algorithm";}
    void tryToMove(int i,MapIndex index,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);

};
#endif //SHIPGIT_UNCORRECTALGORITHM_H
