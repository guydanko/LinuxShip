//
// TODO: on targil 3
//
#include <string>
#include "../interfaces/WeightBalanceCalculator.h"

int WeightBalanceCalculator::readShipPlan(const std::string& full_path_and_file_name){
    return 0;
}
WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y){
    return BalanceStatus::APPROVED;
}