/*algorithm instance that is correct without using MOVE operations*/
#include "NaiveStowageAlgorithm.h"

#ifndef SHIPGIT_MORENAIVEALGORITHM_H
#define SHIPGIT_MORENAIVEALGORITHM_H
using std::string ;
class MoreNaiveAlgorithm: public NaiveStowageAlgorithm{
public:
    MoreNaiveAlgorithm():NaiveStowageAlgorithm(){}

    string getName()const override { return "More Naive algorithm";}
    void moveTower(MapIndex index , const string& portName,shared_ptr<list<shared_ptr<Container>>> rememberLoadAgain, list<CargoOperation>& opList) override ;
};
#endif //SHIPGIT_MORENAIVEALGORITHM_H
