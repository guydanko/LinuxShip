COMP = g++-8.3.0
INTERFACEDIR = ../interfaces/
COMMONDIR = ../common/
OBJS = ../main.o AlgorithmRegistrar.o $(COMMONDIR)CargoOperation.o $(COMMONDIR)Container.o $(COMMONDIR)ShipMap.o $(COMMONDIR)MapIndex.o $(COMMONDIR)FileHandler.o $(INTERFACEDIR)WeightBalanceCalculator.o AlgorithmRegistration.o Simulator.o Travel.o SimulatorError.o SimulatorAlgoCheck.o
EXEC = simulator 
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -ldl -export-dynamic #(-rdynamic)

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

../main.o: ../main.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)CargoOperation.o: $(COMMONDIR)CargoOperation.h $(COMMONDIR)CargoOperation.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)Container.o: $(COMMONDIR)Container.h $(COMMONDIR)Container.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)ShipMap.o: $(COMMONDIR)ShipMap.h $(COMMONDIR)ShipMap.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)MapIndex.o: $(COMMONDIR)MapIndex.h $(COMMONDIR)MapIndex.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(COMMONDIR)FileHandler.o: $(COMMONDIR)FileHandler.h $(COMMONDIR)FileHandler.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
$(INTERFACEDIR)WeightBalanceCalculator.o:  $(INTERFACEDIR)WeightBalanceCalculator.cpp $(INTERFACEDIR)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
Simulator.o: Simulator.h Simulator.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
Travel.o: Travel.h Travel.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
SimulatorError.o: SimulatorError.h SimulatorError.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
SimulatorAlgoCheck.o: SimulatorAlgoCheck.h SimulatorAlgoCheck.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
AlgorithmRegistrar.o: AlgorithmRegistrar.h AlgorithmRegistrar.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp
AlgorithmRegistration.o:  AlgorithmRegistration.cpp
	$(COMP) $(CPP_COMP_FLAG) -o $@ -c $*.cpp


	
clean:
	rm -f $(OBJS) $(EXEC)
