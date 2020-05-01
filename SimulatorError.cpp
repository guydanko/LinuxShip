#include "SimulatorError.h"

ostream& operator<<(ostream& os, const SimulatorError& simulatorError) {
    if(simulatorError.generalError != SimErrorType::OPERATION_PORT){
        os <<  simulatorError.getErrorMessage();
        return os;
    }
    os << simulatorError.getCargoOp() << " because: " << simulatorError.getErrorMessage();
    return os;
}
