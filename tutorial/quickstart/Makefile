
PWD=$(shell pwd)

# set your Simplx path
SIMPLX_PATH=$(PWD)/../..

SRC=$(shell find $(SIMPLX_PATH)/src/simplx_core -iname "*.cpp")
OBJ=$(SRC:.cpp=.o)
INCLUDES= -I$(SIMPLX_PATH)/include
CFLAGS = -g -O0 -Wall -Wextra --std=c++17 -fno-access-control -Wno-unused-parameter -Wno-parentheses -Wno-noexcept-type
LDFLAGS= -lpthread

# here is the name of your executable file
all: quickstart

%.o : %.cpp Makefile
	g++ -c $< -o $@ $(CFLAGS) $(INCLUDES)
	
quickstart : $(OBJ) Makefile $(PWD)/main.o
	g++ -o quickstart main.o $(OBJ) $(LDFLAGS)
	
.PHONY: clean

clean:
	rm -f $(OBJ) main.o quickstart