/*Data struct to represent a calculator that verifies the balance of a ship and containers*/
#include "Ship.h"

#ifndef SHIPGIT_WEIGHTBALANCECALCULATOR_H
#define SHIPGIT_WEIGHTBALANCECALCULATOR_H

enum class BalanceStatus {
    APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
};

class WeightBalanceCalculator {

public:
    WeightBalanceCalculator() {};

    BalanceStatus tryOperation() { return BalanceStatus::APPROVED; }

};

#endif //SHIPGIT_WEIGHTBALANCECALCULATOR_H
