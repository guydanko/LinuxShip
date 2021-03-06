/*Data structure that represents an error found during simulation*/
#include "../common/CargoOperation.h"
#include <fstream>

using std::ofstream;

#ifndef SHIPGIT_SIMULATORERROR_H
#define SHIPGIT_SIMULATORERROR_H

enum class SimErrorType {
    GENERAL_PORT, TRAVEL_END, OPERATION_PORT, TRAVEL_INIT
};


class SimulatorError {
    CargoOperation cargoOp;
    string errorMessage;
    SimErrorType generalError;

public:
    SimulatorError(const string &message, SimErrorType generalError = SimErrorType::OPERATION_PORT,
                   CargoOperation cargoOp = CargoOperation()) : cargoOp(cargoOp), errorMessage(message),
                                                                generalError(generalError) {}

    CargoOperation getCargoOp() const { return this->cargoOp; }

    string getErrorMessage() const { return this->errorMessage; };

    SimErrorType getErrorType() const { return this->generalError; };

    static void
    simulatorErrorsToFile(const std::list<SimulatorError> &simErrors, ofstream & outStream,
                          const string &portName = "", int visitNumber = 0);

    friend ostream &operator<<(ostream &os, const SimulatorError &simulatorError);
};

#endif //SHIPGIT_SIMULATORERROR_H
