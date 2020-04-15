/*working algorithm for stowage operations*/
#include "AbstractStowageAlgorithm.h"
#include "WeightBalanceCalculator.h"

#ifndef SHIPGIT_NAIVESTOWAGEALGORITHM_H
#define SHIPGIT_NAIVESTOWAGEALGORITHM_H

using  std::string ;

class NaiveStowageAlgorithm: public AbstractStowageAlgorithm {

public:
    NaiveStowageAlgorithm(): AbstractStowageAlgorithm() {}
    NaiveStowageAlgorithm(Ship* ship, WeightBalanceCalculator* calculator): AbstractStowageAlgorithm(ship,calculator){}

    list<CargoOperation> getInstructionsForCargo(list<Container*> containerListToLoad,const string& portName) override ;
    void loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList ) override;
    void loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort) override;
    list<Container*>* unloadContainerByPort(const string& portName, list<CargoOperation>& opList ) override;
    virtual void moveTower(MapIndex index , const string& portName,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);
    void loadOneContainer(Container* cont, list<CargoOperation>& opList);
    string getName()const override { return "Naive algorithm";}
    void tryToMove(int i,MapIndex index ,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);

};


#endif //SHIPGIT_NAIVESTOWAGEALGORITHM_H
