/*algorithm instance that is correct without using MOVE operations*/
#include "_316294636_a.h"

#ifndef SHIPGIT_MORENAIVEALGORITHM_H
#define SHIPGIT_MORENAIVEALGORITHM_H
using std::string ;
class _316294636_b: public _316294636_a{
public:
    _316294636_b(): _316294636_a(){}

    string getName()const override { return "More Naive algorithm";}
    void moveTower(MapIndex index , const string& portName,list<shared_ptr<Container>>& rememberLoadAgain, list<CargoOperation>& opList) override ;
};
#endif //SHIPGIT_MORENAIVEALGORITHM_H
