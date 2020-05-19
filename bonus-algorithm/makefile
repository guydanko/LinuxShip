COMP = g++-9.3.0
COMMONDIR = ../common/

SHARED_OBJS =  IncorrectAlgorithm.so
OBJSCOMMON = CargoOperation.o  Container.o  ShipMap.o  MapIndex.o FileHandler.o
OBJS =  ../algorithm/AbstractCommonAlgorithm.o IncorrectAlgorithm.o
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -lstdc++fs -shared


all: $(SHARED_OBJS) $(OBJSCOMMON)
$(OBJSCOMMON):
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMONDIR)/$*.cpp -o $@

AbstractCommonAlgorithm.o: ../algorithm/AbstractCommonAlgorithm.cpp ../algorithm/AbstractCommonAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
IncorrectAlgorithm.so: IncorrectAlgorithm.o $(OBJSCOMMON) ../algorithm/AbstractCommonAlgorithm.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
IncorrectAlgorithm.o: IncorrectAlgorithm.cpp IncorrectAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
clean:
	rm -f $(OBJS) $(SHARED_OBJS) $(OBJSCOMMON)