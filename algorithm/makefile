COMP = g++-9.3.0
COMMONDIR = ../common/

SHARED_OBJS = _316294636_a.so _316294636_b.so IncorrectAlgorithm.so
OBJSCOMMON = CargoOperation.o  Container.o  ShipMap.o  MapIndex.o FileHandler.o
OBJS = _316294636_a.o _316294636_b.o AbstractCommonAlgorithm.o IncorrectAlgorithm.o
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden
CPP_LINK_FLAG = -lstdc++fs -shared


all: $(SHARED_OBJS) $(OBJSCOMMON)
$(OBJSCOMMON):
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMONDIR)/$*.cpp -o $@

AbstractCommonAlgorithm.o: AbstractCommonAlgorithm.cpp AbstractCommonAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
_316294636_a.so: _316294636_a.o $(OBJSCOMMON) AbstractCommonAlgorithm.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_316294636_a.o: _316294636_a.cpp _316294636_a.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
_316294636_b.so: _316294636_b.o $(OBJSCOMMON) AbstractCommonAlgorithm.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_316294636_b.o: _316294636_b.cpp _316294636_b.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
IncorrectAlgorithm.so: IncorrectAlgorithm.o $(OBJSCOMMON) AbstractCommonAlgorithm.o
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
IncorrectAlgorithm.o: IncorrectAlgorithm.cpp IncorrectAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp
clean:
	rm -f $(OBJS) $(SHARED_OBJS) $(OBJSCOMMON)