COMP = g++-8.3.0
OBJS = main.o Container.o ShipMap.o Ship.o MapIndex.o FileHandler.o Travel.o SimulatorError.o WeightBalanceCalculator.o CargoOperation.o AbstractStowageAlgorithm.o NaiveStowageAlgorithm.o MoreNaiveAlgorithm.o IncorrectAlgorithm.o Simulator.o

EXEC = ex1

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -lstdc++fs

EXEC = ex1

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
SimulatorError.o: SimulatorError.h SimulatorError.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Container.o: Container.h Container.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
ShipMap.o: ShipMap.h ShipMap.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Ship.o: Ship.h Ship.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
MapIndex.o: MapIndex.h MapIndex.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
FileHandler.o: FileHandler.h FileHandler.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Travel.o: Travel.h Travel.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
WeightBalanceCalculator.o: WeightBalanceCalculator.h WeightBalanceCalculator.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
CargoOperation.o: CargoOperation.h CargoOperation.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
AbstractStowageAlgorithm.o: AbstractStowageAlgorithm.h AbstractStowageAlgorithm.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
NaiveStowageAlgorithm.o: NaiveStowageAlgorithm.h NaiveStowageAlgorithm.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
MoreNaiveAlgorithm.o: MoreNaiveAlgorithm.h MoreNaiveAlgorithm.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
IncorrectAlgorithm.o: IncorrectAlgorithm.h IncorrectAlgorithm.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
Simulator.o: Simulator.h Simulator.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)