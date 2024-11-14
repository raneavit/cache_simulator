CC = g++
OPT = -O3
# OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc/.cpp files here (source files, excluding header files)
SIM_SRC = sim.cc
SIM_SRC += addressParser.cc
SIM_SRC += cacheModule.cc
SIM_SRC += prefetchModule.cc

# List corresponding compiled object files here (.o files)
SIM_OBJ = sim.o
SIM_OBJ += addressParser.o
SIM_OBJ += cacheModule.o
SIM_OBJ += prefetchModule.o

#################################

# default rule

all: sim
	@echo "my work is done here..."


# rule for making sim

sim: $(SIM_OBJ)
	$(CC) -o sim $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH sim-----------"


# generic rule for converting any .cc file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS) -c $*.cc

# generic rule for converting any .cpp file to any .o file

.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp


# type "make clean" to remove all .o files plus the sim binary

clean:
	rm -f *.o sim


# type "make clobber" to remove all .o files (leaves sim binary)

clobber:
	rm -f *.o


