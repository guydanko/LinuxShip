#include "SimulatorError.h"

ostream& operator<<(ostream& os, const SimulatorError& simulatorError) {
    if(simulatorError.generalError){
        os << "Simulator Error: " << simulatorError.getErrorMessage();
        return os;
    }
    os << "Simulator Error: " << simulatorError.getCargoOp() << " because: " << simulatorError.getErrorMessage();
    return os;
}
