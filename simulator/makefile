COMP = g++-9.3.0
INTERFACEDIR = ../interfaces
COMMONDIR = ../common
OBJSCOMMON = CargoOperation.o  Container.o  ShipMap.o  MapIndex.o FileHandler.o
OBJSIMULATOR =   main.o AlgorithmRegistrar.o WeightBalanceCalculator.o AlgorithmRegistration.o Simulator.o Travel.o SimulatorError.o SimulatorAlgoCheck.o Producer.o
EXEC = simulator
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -lstdc++fs -ldl -export-dynamic -pthread

$(EXEC): $(OBJSCOMMON) $(OBJSIMULATOR)
	$(COMP)  $(OBJSCOMMON) $(OBJSIMULATOR) $(CPP_LINK_FLAG) -o $@

$(OBJSCOMMON):
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMONDIR)/$*.cpp -o $@
$(OBJSIMULATOR):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@

clean:
	rm -f $(OBJSCOMMON) $(OBJSIMULATOR) $(EXEC)
