/*algorithm instance that is correct without using MOVE operations*/
#include "NaiveStowageAlgorithm.h"

#ifndef SHIPGIT_MORENAIVEALGORITHM_H
#define SHIPGIT_MORENAIVEALGORITHM_H
using std::string ;
class MoreNaiveAlgorithm: public NaiveStowageAlgorithm{
public:
    MoreNaiveAlgorithm():NaiveStowageAlgorithm(){}
    MoreNaiveAlgorithm(Ship* ship,WeightBalanceCalculator* calculator):NaiveStowageAlgorithm(ship,calculator){}
    string getName()const override{ return "More Naive algorithm";}
    void moveTower(MapIndex index , const string& portName,list<Container*>* rememberLoadAgain, list<CargoOperation>& opList) override ;
};
#endif //SHIPGIT_MORENAIVEALGORITHM_H
