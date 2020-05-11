COMP = g++-8.3.0
INTERFACEDIR = ../interfaces/
COMMONDIR = ../common/

SHARED_OBJS = _316294636_a.so _316294636_b.so
OBJS = _316294636_a.o _316294636_b.o
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

_316294636_a.so: $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_316294636_a.o: _316294636_a.cpp _316294636_a.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 
_316294636_b.so: $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_316294636_b.o: _316294636_b.cpp _316294636_b.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp 
clean:
	rm -f $(OBJS) $(SHARED_OBJS)
