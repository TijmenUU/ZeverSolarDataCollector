# Build settings
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -std=c++11 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Build specific settings
COLLECTORBUILDNAME = collector
COLLECTOROBJS = fileutils.o timeutils.o configuration.o zeverdata.o collector.o
COLLECTORLIBS = -lcurl

DECIMALFIXBUILDNAME = decimalfixer
DECIMALFIXOBJS = timeutils.o configuration.o decimalfixer.o
DECIMALFIXLIBS =

fileutils.o: fileutils.hpp
	$(CC) $(CFLAGS) fileutils.cpp

timeutils.o: timeutils.hpp
	$(CC) $(CFLAGS) timeutils.cpp

configuration.o: configuration.hpp timeutils.hpp
	$(CC) $(CFLAGS) configuration.cpp

zeverdata.o: zeverdata.hpp timeutils.hpp
	$(CC) $(CFLAGS) zeverdata.cpp

collector.o: fileutils.hpp configuration.hpp zeverdata.hpp
	$(CC) $(CFLAGS) collector.cpp

decimalfixer.o : configuration.hpp
	$(CC) $(CFLAGS) decimalfixer.cpp

collector: $(COLLECTOROBJS)
	$(CC) $(LFLAGS) $(COLLECTOROBJS) -o $(COLLECTORBUILDNAME) $(COLLECTORLIBS)

decimalfixer: $(DECIMALFIXOBJS)
	$(CC) $(LFLAGS) $(DECIMALFIXOBJS) -o $(DECIMALFIXBUILDNAME) $(DECIMALFIXLIBS)

all: collector decimalfixer

clean:
	\rm *.o $(COLLECTORBUILDNAME) $(DECIMALFIXBUILDNAME)
