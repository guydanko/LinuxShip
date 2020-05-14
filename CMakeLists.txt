cmake_minimum_required(VERSION 3.15)
project(LinuxShip)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic-errors -DNDEBUG")

add_executable(LinuxShip simulator/main.cpp common/Container.h common/Container.cpp common/ShipMap.cpp common/ShipMap.h common/CargoOperation.cpp common/CargoOperation.h simulator/Simulator.cpp simulator/Simulator.h algorithms/_316294636_a.cpp algorithms/_316294636_a.h algorithms/_316294636_b.h algorithms/_316294636_b.cpp common/MapIndex.h
        common/MapIndex.cpp common/FileHandler.cpp common/FileHandler.h simulator/Travel.h simulator/Travel.cpp interfaces/WeightBalanceCalculator.h simulator/WeightBalanceCalculator.cpp
        simulator/SimulatorError.cpp simulator/SimulatorError.h interfaces/AbstractAlgorithm.h simulator/SimulatorAlgoCheck.cpp simulator/SimulatorAlgoCheck.h interfaces/AlgorithmRegistration.h simulator/AlgorithmRegistrar.cpp simulator/AlgorithmRegistrar.h)