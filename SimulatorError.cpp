#include "SimulatorError.h"

ostream& operator<<(ostream& os, const SimulatorError& simulatorError) {
    if(simulatorError.generalError == SimErrorType::GENERAL_PORT){
        os << "Simulator Error: " << simulatorError.getErrorMessage();
        return os;
    }
    os << "Simulator Error: " << simulatorError.getCargoOp() << " because: " << simulatorError.getErrorMessage();
    return os;
}
