//
// TODO: on targil 3
//
#include <string>
#include "../interfaces/WeightBalanceCalculator.h"

int WeightBalanceCalculator::readShipPlan(const std::string& full_path_and_file_name){
    if(full_path_and_file_name==""){
        return 0;
    }
    return 0;
}
WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int X, int Y){
    //Naive implemenatation of tryOperation - WILL ALWAYS RETURN APPROVED
    if( loadUnload=='f' || kg+X+Y==4){
        return BalanceStatus::APPROVED;
    }
    return BalanceStatus::APPROVED;
}