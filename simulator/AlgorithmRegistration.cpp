#include "../interfaces/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

using std::function;

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory){
    AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}