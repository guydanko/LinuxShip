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
SimulatorError::simulatorErrorsToFile(const std::list<SimulatorError> &simErrors, const string &path,
                                      const string &travelName,
                                      const string &portName, int visitNumber,
                                      const string &errorFileName) {
    ofstream outFile;
    outFile.open(path, std::ios::app);
    ofstream errorFile(errorFileName, std::ios::app);
    if (!outFile) {
        errorFile << "Could not write error file: " << path + "/" + travelName + "AlgoErrors" << "\n";
        errorFile.close();
        return;
    }

    if (!simErrors.empty() && simErrors.front().getErrorType() != SimErrorType::TRAVEL_END &&
        simErrors.front().getErrorType() != SimErrorType::TRAVEL_INIT) {
        outFile << "Simulation Errors in port: " << portName << " ,visit no: " << visitNumber << "\n";
    }

    for (const SimulatorError &simError:simErrors) {
        outFile << simError << "\n";
    }

    if (!simErrors.empty()) {
        outFile << "===========================================================================================\n";
    }

    errorFile.close();
    outFile.close();
}
