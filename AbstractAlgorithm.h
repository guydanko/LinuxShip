//
// Created by noada on 4/29/2020.
//

#ifndef LINUXSHIP_ABSTRACTALGORITHM_H
#define LINUXSHIP_ABSTRACTALGORITHM_H

#include <string>

using std::string;

#pragma once
class AbstractAlgorithm {
public:
    virtual ~AbstractAlgorithm(){}
    enum class Action { LOAD = 'L', UNLOAD = 'U', MOVE = 'M', REJECT = 'R'};
// methods below return int for returning 0 for success
// and any other number as error code
    virtual int readShipPlan(const std::string& full_path_and_file_name) = 0;
    virtual int readShipRoute(const std::string& full_path_and_file_name) = 0;
    virtual int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) = 0;
    virtual int getInstructionsForCargo(
            const std::string& input_full_path_and_file_name,
            const std::string& output_full_path_and_file_name) = 0;
};
#endif //LINUXSHIP_ABSTRACTALGORITHM_H
