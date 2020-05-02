cmake_minimum_required(VERSION 3.15)
project(LinuxShip)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Wextra -Werror -pedantic-errors -DNDEBUG")

add_executable(LinuxShip main.cpp Ship.h Ship.cpp Container.h Container.cpp ShipMap.cpp ShipMap.h CargoOperation.cpp CargoOperation.h Simulator.cpp Simulator.h NaiveStowageAlgorithm.cpp NaiveStowageAlgorithm.h MoreNaiveAlgorithm.h MoreNaiveAlgorithm.cpp MapIndex.h
        MapIndex.cpp FileHandler.cpp FileHandler.h Travel.h Travel.cpp WeightBalanceCalculator.h WeightBalanceCalculator.cpp
        SimulatorError.cpp SimulatorError.h IncorrectAlgorithm.h IncorrectAlgorithm.cpp AbstractAlgorithm.h SimulatorAlgoCheck.cpp SimulatorAlgoCheck.h AlgoTravelPair.cpp AlgoTravelPair.h)