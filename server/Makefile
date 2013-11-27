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
		thrift -r -gen cpp:pure_enums -I ../../astrothrift ../swissguard.thrift 

swissguard: swissguard_constants.o swissguard_types.o SwissGuardService.o SwissGuardHandler.o astro_constants.o astro_types.o AstroThriftService.o 
		$(CC) $(LDFLAGS) SwissGuardHandler.o SwissGuardService.o swissguard_constants.o swissguard_types.o AstroThriftService.o astro_constants.o astro_types.o -o swissguard_server

astro_constants.o: gen-cpp/astro_constants.cpp
		$(CC) $(CFLAGS) gen-cpp/astro_constants.cpp

astro_types.o: gen-cpp/astro_types.cpp
		$(CC) $(CFLAGS) gen-cpp/astro_types.cpp

AstroThriftService.o: gen-cpp/AstroThriftService.cpp
		$(CC) $(CFLAGS) gen-cpp/AstroThriftService.cpp

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

