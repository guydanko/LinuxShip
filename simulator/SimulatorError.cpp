#include "SimulatorError.h"

ostream &operator<<(ostream &os, const SimulatorError &simulatorError) {
    if (simulatorError.generalError != SimErrorType::OPERATION_PORT) {
        os << simulatorError.getErrorMessage();
        return os;
    }
    os << simulatorError.getCargoOp() << " because: " << simulatorError.getErrorMessage();
    return os;
}

void
SimulatorError::simulatorErrorsToFile(const std::list<SimulatorError> &simErrors, ofstream & outStream,
                                      const string &travelName,
                                      const string &portName, int visitNumber) {

    if (!outStream) {
        return;
    }

    if (!simErrors.empty() && simErrors.front().getErrorType() != SimErrorType::TRAVEL_END &&
        simErrors.front().getErrorType() != SimErrorType::TRAVEL_INIT) {
        outStream << "Simulation Errors in port: " << portName << " ,visit no: " << visitNumber << "\n";
    }

    for (const SimulatorError &simError:simErrors) {
        outStream << simError << "\n";
    }

    if (!simErrors.empty()) {
        outStream << "===========================================================================================\n";
    }

}
