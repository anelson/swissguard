LIBS = -lthrift
CC = cc
CFLAGS = -g -Wall
THRIFT = thrif

default: all
all: gen-server gen-client swissguard

gen-server: 
