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
		thrift -r -gen cpp:pure_enums ../swissguard.thrift 

swissguard: swissguard_constants.o swissguard_types.o SwissGuardService.o SwissGuardHandler.o
		$(CC) $(LDFLAGS) SwissGuardHandler.o SwissGuardService.o swissguard_constants.o swissguard_types.o -o swissguard_server

swissguard_constants.o: gen-cpp/swissguard_constants.cpp
		$(CC) $(CFLAGS) gen-cpp/swissguard_constants.cpp

swissguard_types.o: gen-cpp/swissguard_types.cpp
		$(CC) $(CFLAGS) gen-cpp/swissguard_types.cpp

swissguard_constants.o: gen-cpp/swissguard_constants.cpp
		$(CC) $(CFLAGS) gen-cpp/swissguard_constants.cpp

swissguard_types.o: gen-cpp/swissguard_types.cpp
		$(CC) $(CFLAGS) gen-cpp/swissguard_types.cpp

SwissGuardService.o: gen-cpp/SwissGuardService.cpp
		$(CC) $(CFLAGS) gen-cpp/SwissGuardService.cpp

SwissGuardHandler.o: SwissGuardHandler.cpp
		$(CC) $(CFLAGS) SwissGuardHandler.cpp

clean:
		rm -rf *o swissguard_server
		rm -rf gen-cpp/

