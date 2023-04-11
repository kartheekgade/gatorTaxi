# Makefile for Writing Make Files Example
 
# *****************************************************
# Variables to control Makefile operation
 
#CC = g++
#CFLAGS = -Wall -g
 
# ****************************************************
# Targets needed to bring the executable up to date

PROGRAM = gatorTaxi
CXX = g++
CXXFLAGS = -std=c++0x

SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = $(wildcard *.h)

default: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${PROGRAM}

clean:
	rm -f ${OBJECTS} ${PROGRAM}