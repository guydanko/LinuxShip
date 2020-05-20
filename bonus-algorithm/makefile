COMP = g++-9.3.0
COMMONDIR = ../common/
ALGODIR = ../algorithm/

SHARED_OBJS = IncorrectAlgorithm.so
OBJSCOMMON = CargoOperation.o  Container.o  ShipMap.o  MapIndex.o FileHandler.o
ALGOOBJS = AbstractCommonAlgorithm.o
OBJS = IncorrectAlgorithm.o 
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -lstdc++fs -shared


all: $(SHARED_OBJS) $(OBJSCOMMON) $(ALGOOBJS)
$(OBJSCOMMON):
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMONDIR)/$*.cpp -o $@
$(ALGOOBJS):
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGODIR)/$*.cpp -o $@

IncorrectAlgorithm.so: IncorrectAlgorithm.o $(ALGOOBJS) $(OBJSCOMMON)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
IncorrectAlgorithm.o: IncorrectAlgorithm.cpp IncorrectAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(SHARED_OBJS) $(OBJSCOMMON) $(ALGOOBJS)