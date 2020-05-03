/*working algorithm for stowage operations*/

#include "WeightBalanceCalculator.h"
#include "AbstractAlgorithm.h"
#include "MapIndex.h"
#include "CargoOperation.h"
#include <memory>


#ifndef SHIPGIT_NAIVESTOWAGEALGORITHM_H
#define SHIPGIT_NAIVESTOWAGEALGORITHM_H

using std::shared_ptr;
using std::string;
using std::list;

class NaiveStowageAlgorithm : public AbstractAlgorithm {

protected:
    shared_ptr<ShipMap> shipMap;
    list<string> route;
    WeightBalanceCalculator calculator;
public:
    NaiveStowageAlgorithm() : AbstractAlgorithm() {}

    void loadAgain(shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain, list<CargoOperation> &opList);

    void loadNewContainers(list<shared_ptr<Container>> &containerListToLoad, list<CargoOperation> &opList,
                           const string &currentPort);

    shared_ptr<list<shared_ptr<Container>>> unloadContainerByPort(const string &portName, list<CargoOperation> &opList);

    virtual void
    moveTower(MapIndex index, const string &portName, shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain,
              list<CargoOperation> &opList);

    void loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList);

    virtual string getName() const { return "Naive algorithm"; }

    void tryToMove(int i, MapIndex index, shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain,
                   list<CargoOperation> &opList);

    int rejectDoubleId(list<shared_ptr<Container>> &containerListToLoadInThisPort, list<CargoOperation> &opList);

    //new func
    int readShipPlan(const std::string &full_path_and_file_name) override;

    int readShipRoute(const std::string &full_path_and_file_name) override;

    int setWeightBalanceCalculator(WeightBalanceCalculator &calculator) override;

    int getInstructionsForCargo(const std::string &input_full_path_and_file_name,
                                const std::string &output_full_path_and_file_name) override;
};


#endif //SHIPGIT_NAIVESTOWAGEALGORITHM_H
