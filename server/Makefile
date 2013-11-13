SWEPATH=/Users/adam/sources/swissephem/src
THRIFT = thrift
THRIFTINC = /usr/local/include/thrift

#CC=cc
CC=clang++
#CFLAGS=-g -O3 -Wall -I$(SWEPATH) -I$(THRIFTINC) -c
CFLAGS=-std=c++11 -Wall -g -O3 -I$(SWEPATH) -I$(THRIFTINC) -c 
 
LDFLAGS=-std=c++11 -lthrift -lswe -L$(SWEPATH) -v

all: thrift swissguard

thrift:
		thrift -gen cpp:pure_enums ../swissguard.thrift
		cp gen-cpp/*.cpp .
		cp gen-cpp/*.h .

swissguard: swissguard_constants.o swissguard_types.o SwissGuard.o SwissGuardHandler.o 
		$(CC) $(LDFLAGS) SwissGuardHandler.o SwissGuard.o swissguard_constants.o swissguard_types.o -o swissguard_server

swissguard_constants.o: swissguard_constants.cpp
		$(CC) $(CFLAGS) swissguard_constants.cpp

swissguard_types.o: swissguard_types.cpp
		$(CC) $(CFLAGS) swissguard_types.cpp

SwissGuard.o: SwissGuard.cpp
		$(CC) $(CFLAGS) SwissGuard.cpp

SwissGuardHandler.o: SwissGuardHandler.cpp
		$(CC) $(CFLAGS) SwissGuardHandler.cpp

clean:
		rm -rf *o swissguard
		mv SwissGuardHandler.cpp SwissGuardHandler.tmp
		rm *.cpp
		rm *.h
		cp SwissGuardHandler.tmp SwissGuardHandler.cpp
		rm SwissGuardHandler.tmp
		rm -rf gen-cpp/