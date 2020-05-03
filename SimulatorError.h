/*Data structure that represents an error found during simulation*/
#include "CargoOperation.h"

using std::string;

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

    friend ostream &operator<<(ostream &os, const SimulatorError &simulatorError);
};

#endif //SHIPGIT_SIMULATORERROR_H
