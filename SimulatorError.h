#include "CargoOperation.h"
using std::string;

#ifndef SHIPGIT_SIMULATORERROR_H
#define SHIPGIT_SIMULATORERROR_H

class SimulatorError{
    CargoOperation cargoOp;
    string errorMessage;
    int generalError = 0; //if 1 do not touch cargoOp


public:
    SimulatorError( const string& message, int generalError=0,CargoOperation cargoOp=CargoOperation()): cargoOp(cargoOp), errorMessage(message), generalError(generalError){}
    CargoOperation getCargoOp()const{ return this->cargoOp;}
    string getErrorMessage() const { return this->errorMessage;};
    friend ostream& operator<<(ostream& os, const SimulatorError& simulatorError);
};
#endif //SHIPGIT_SIMULATORERROR_H
