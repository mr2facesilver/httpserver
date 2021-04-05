# Makefile for compiling a C++ program with the Lua library
CXX=g++ 
CXXFLAGS= -g -I/usr/include/lua5.2
#LDFLAGS=-g -L/usr/lib/x86_64-linux-gnu -Wl,-Bstatic -llua5.2
LDFLAGS=-g -L/usr/lib/x86_64-linux-gnu -llua5.2
LINK.o = $(LINK.cc)

PROG=dofile_ex

all: $(PROG)

$(PROG): $(PROG).o

$(PROG).o: $(PROG).cpp

.PHONY: clean depend
clean: 
	rm -f $(PROG) $(PROG).o

depend:
	touch make.depend
	$(CXX) -M $(CXXFLAGS) -M $(PROG).cpp > make.depend

-include maked.depend
