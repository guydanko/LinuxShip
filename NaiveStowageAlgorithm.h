/*working algorithm for stowage operations*/
#include "AbstractAlgorithm.h"
#include "WeightBalanceCalculator.h"
#include "MapIndex.h"
#include "CargoOperation.h"

#ifndef SHIPGIT_NAIVESTOWAGEALGORITHM_H
#define SHIPGIT_NAIVESTOWAGEALGORITHM_H

using  std::string ;

class NaiveStowageAlgorithm: public AbstractAlgorithm {

protected:
    Ship *ship = nullptr;
    WeightBalanceCalculator calculator;
public:
    NaiveStowageAlgorithm(): AbstractAlgorithm() {}
    NaiveStowageAlgorithm(Ship* ship, WeightBalanceCalculator calculator): AbstractAlgorithm(){}

    void loadAgain(list<Container*>* rememberLoadAgain, list<CargoOperation>& opList ) ;
    void loadNewContainers(list<Container*>& containerListToLoad, list<CargoOperation>& opList, const string& currentPort) ;
    list<Container*>* unloadContainerByPort(const string& portName, list<CargoOperation>& opList ) ;
    virtual void moveTower(MapIndex index , const string& portName,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);
    void loadOneContainer(Container* cont, list<CargoOperation>& opList);
    virtual string getName()const  { return "Naive algorithm";}
    void tryToMove(int i,MapIndex index ,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList);

    //new func
    int readShipPlan(const std::string& full_path_and_file_name) override ;
    int readShipRoute(const std::string& full_path_and_file_name) override;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(const std::string& input_full_path_and_file_name, const std::string& output_full_path_and_file_name) override;
};


#endif //SHIPGIT_NAIVESTOWAGEALGORITHM_H
