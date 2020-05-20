/*working algorithm for stowage operations*/
#include "AbstractCommonAlgorithm.h"

#ifndef SHIPGIT_NAIVESTOWAGEALGORITHM_H
#define SHIPGIT_NAIVESTOWAGEALGORITHM_H

class _316294636_a : public AbstractCommonAlgorithm {


public:
    _316294636_a() : AbstractCommonAlgorithm() {}

    virtual void
    moveTower(MapIndex index, const string &portName, list<shared_ptr<Container>>& rememberLoadAgain,
              list<CargoOperation> &opList) override ;

};


#endif //SHIPGIT_NAIVESTOWAGEALGORITHM_H
