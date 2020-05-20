/*Data struct to represent a calculator that verifies the balance of a ship and containers*/

#ifndef SHIPGIT_WEIGHTBALANCECALCULATOR_H
#define SHIPGIT_WEIGHTBALANCECALCULATOR_H

// WeightBalanceCalculator.h

#pragma once

class WeightBalanceCalculator {
public:

    enum BalanceStatus {
        APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
    };

// EDIT, 05-05-2020L: the method below is required even for ex2
// but you may implement it just as: return 0;
    int readShipPlan(const std::string& full_path_and_file_name);

    BalanceStatus tryOperation(char loadUnload, int kg, int X, int Y);
};

#endif //SHIPGIT_WEIGHTBALANCECALCULATOR_H
