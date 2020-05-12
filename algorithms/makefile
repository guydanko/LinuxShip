COMP = g++-8.3.0
COMMONDIR = ../common/

SHARED_OBJS = _316294636_a.so
OBJSCOMMON = CargoOperation.o  Container.o  ShipMap.o  MapIndex.o FileHandler.o
OBJS = _316294636_a.o
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -lstdc++fs -shared


all: $(SHARED_OBJS) $(OBJSCOMMON)
$(OBJSCOMMON):
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMONDIR)/$*.cpp -o $@

_316294636_a.so: $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_316294636_a.o: _316294636_a.cpp _316294636_a.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 
clean:
	rm -f $(OBJS) $(SHARED_OBJS) $(OBJSCOMMON)