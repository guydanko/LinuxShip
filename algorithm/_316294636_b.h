/*working algorithm for stowage operations*/
#include "AbstractCommonAlgorithm.h"

#ifndef SHIPGIT_NAIVESTOWAGEALGORITHM_H
#define SHIPGIT_NAIVESTOWAGEALGORITHM_H

class _316294636_b : public AbstractCommonAlgorithm {

public:
    _316294636_b() : AbstractCommonAlgorithm() {}
    int loadOneContainer(shared_ptr<Container> cont, list<CargoOperation> &opList) override ;

};

#endif //SHIPGIT_NAIVESTOWAGEALGORITHM_H
