# TODO: Add in debug and release targets, and put in optimisation
# for release build, and symbols for debug build.
# this might point the way:
# https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds
# vscode task was got from here: https://gist.github.com/Hirosam1/a49fd002a365c976d3201a9c9d5f6918
# need to tweak to get the two building the right target.

# example from https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# need to use g++ not gc as g++ knows where C++ stdlib files are

I2CDIR := ./lib/I2Cdev
MPU6050DIR := ./lib/MPU6050
CXX := g++
CXXFLAGS := -g -I$(I2CDIR) -I$(MPU6050DIR)

ODIR=obj
LDIR =../lib

LIBS=-lwiringPi -lm

OUTFILE = sbrcontrold

_DEPS = ISpacialSensor.h MPU6050.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = MPU6050.o sbrmain.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	mkdir -p $(ODIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

debug: $(OBJ)
	$(CXX) -o $(OUTFILE) $^ $(LIBS) $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~