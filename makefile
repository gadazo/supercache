
CC = g++
CXXFLAGS = -g -Wall -std=c++0x
CCLINK = $(CC)
LIBS =
OBJS = cacheSim.o cache.o
RM = rm -f
cacheSim: $(OBJS)
	$(CCLINK) -o cacheSim $(OBJS) $(LIBS)

cacheSim.o: cacheSim.cpp cache.h
cache.o: cache.cpp cache.h

clean:
	rm -f *.o
	rm -f cacheSim
