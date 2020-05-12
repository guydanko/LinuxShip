#include "../interfaces/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

using std::function;

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> algorithmFactory){
    std::cout << "AlgorithmRegistration constructor.." << &algorithmFactory << "\n";
    AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}