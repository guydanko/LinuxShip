#include "../interfaces/AlgorithmRegistration.h"

using std::function;

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>){

}
