# Build settings
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -std=c++11 $(DEBUG)
LFLAGS = -Wall $(DEBUG)

# Build specific settings
COLLECTORBUILDNAME = collector
WIN_COLLECTORBUILDNAME = collector.exe
COLLECTOROBJS = fileutils.o timeutils.o configuration.o zeverdata.o collector.o
COLLECTORLIBS = -lcurl
WIN_COLLECTORLIBS = -llibcurl

DECIMALFIXBUILDNAME = decimalfixer
WIN_DECIMALFIXBUILDNAME = decimalfixer.exe
DECIMALFIXOBJS = timeutils.o configuration.o decimalfixer.o
DECIMALFIXLIBS =
WIN_DECIMALFIXLIBS =

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

win_collector: $(COLLECTOROBJS)
	$(CC) $(LFLAGS) $(COLLECTOROBJS) -o $(WIN_COLLECTORBUILDNAME) $(WIN_COLLECTORLIBS)

decimalfixer: $(DECIMALFIXOBJS)
	$(CC) $(LFLAGS) $(DECIMALFIXOBJS) -o $(DECIMALFIXBUILDNAME) $(DECIMALFIXLIBS)

win_decimalfixer: $(DECIMALFIXOBJS)
	$(CC) $(LFLAGS) $(DECIMALFIXOBJS) -o $(WIN_DECIMALFIXBUILDNAME) $(WIN_DECIMALFIXLIBS)

all: collector decimalfixer

win_all: win_collector win_decimalfixer

clean:
	\rm *.o $(COLLECTORBUILDNAME) $(DECIMALFIXBUILDNAME)

win_clean:
	del *.o
	del $(WIN_COLLECTORBUILDNAME)
	del $(WIN_DECIMALFIXBUILDNAME)
