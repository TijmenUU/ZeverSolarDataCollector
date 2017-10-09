OBJS = fileutils.o timeutils.o configuration.o zeverdata.o main.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -std=c++11 $(DEBUG)
LFLAGS = -Wall $(DEBUG)
LIBS = -lcurl

fileutils.o: fileutils.hpp
	$(CC) $(CFLAGS) fileutils.cpp

timeutils.o: timeutils.hpp
	$(CC) $(CFLAGS) timeutils.cpp

configuration.o: configuration.hpp timeutils.hpp
	$(CC) $(CFLAGS) configuration.cpp

zeverdata.o: zeverdata.hpp timeutils.hpp
	$(CC) $(CFLAGS) zeverdata.cpp

main.o: fileutils.hpp configuration.hpp zeverdata.hpp
	$(CC) $(CFLAGS) main.cpp

test: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o test $(LIBS)

clean:
	\rm *.o test
