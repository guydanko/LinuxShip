/*Data struct to represent a calculator that verifies the balance of a ship and containers*/
#include "Ship.h"

#ifndef SHIPGIT_WEIGHTBALANCECALCULATOR_H
#define SHIPGIT_WEIGHTBALANCECALCULATOR_H

// WeightBalanceCalculator.h
#pragma once
class WeightBalanceCalculator {

public:
    enum BalanceStatus {
        APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
    };
// the method below is optional for ex2
// int readShipPlan(const std::string& full_path_and_file_name);
    BalanceStatus tryOperation(char loadUnload, int kg, int X, int Y){ return BalanceStatus ::APPROVED;};
};

#endif //SHIPGIT_WEIGHTBALANCECALCULATOR_H
