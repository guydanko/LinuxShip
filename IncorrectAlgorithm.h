/*incorrect algorithm to verify that simulator works*/
//
// this algorithm forget container on the port
// load container while id is already taken
// load container with destination which is not in route
// don't check balance with calculator
// load in arbitrary order on port destination
//

#include "AbstractAlgorithm.h"
#include "WeightBalanceCalculator.h"
#include "CargoOperation.h"

#ifndef SHIPGIT_INCORRECTALGORITHM_H
#define SHIPGIT_INCORRECTALGORITHM_H

using std::string;

class IncorrectAlgorithm : public AbstractAlgorithm {
protected:
    Ship *ship = nullptr;
    WeightBalanceCalculator calculator;
public:
    IncorrectAlgorithm() : AbstractAlgorithm() {}
    IncorrectAlgorithm(Ship* ship, WeightBalanceCalculator calculator): AbstractAlgorithm(){}
        //new func
    int readShipPlan(const std::string& full_path_and_file_name) override ;
    int readShipRoute(const std::string& full_path_and_file_name) override;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(const std::string& input_full_path_and_file_name, const std::string& output_full_path_and_file_name) override;



    void loadNewContainers(list<Container *> &containerListToLoad, list<CargoOperation> &opList,
                           const string &currentPort) ;

    list<Container *> *unloadContainerByPort(const string &portName, list<CargoOperation> &opList) ;

    virtual void moveTower(MapIndex index, const string &portName, list<CargoOperation> &opList);

    void loadOneContainer(Container *cont, list<CargoOperation> &opList);

    string getName() const  { return "Incorrect algorithm"; }

    void tryToMove(int i, MapIndex index, list<CargoOperation> &opList);

};

#endif //SHIPGIT_INCORRECTALGORITHM_H
