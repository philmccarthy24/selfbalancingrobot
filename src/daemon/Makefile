# TODO: Add in debug and release targets, and put in optimisation
# for release build, and symbols for debug build.
# this might point the way:
# https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds
# vscode task was got from here: https://gist.github.com/Hirosam1/a49fd002a365c976d3201a9c9d5f6918
# need to tweak to get the two building the right target.

# example from https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# need to use g++ not gc as g++ knows where C++ stdlib files are

.DEFAULT_GOAL := release

CXX := g++
CXXFLAGS := -I./ -Ilib/I2Cdev -Ilib/MPU6050

VPATH=lib/I2Cdev:lib/MPU6050

ODIR=obj
LDIR=
LIBS=-lbcm2835
OUTFILE = sbrcontrollerd

DEPS = ISpacialSensor.h lib/I2Cdev/I2Cdev.h lib/MPU6050/MPU6050.h

_OBJ = main.o I2Cdev.o MPU6050.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	@mkdir -p $(@D)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

debug: CXXFLAGS += -DDEBUG -g
debug: OUTDIR = bin/debug
debug: executable

release: CXXFLAGS += -O2 -Wall
release: OUTDIR = bin/release
release: executable

executable: $(OBJ)
	@mkdir -p $(OUTDIR)
	$(CXX) -o $(OUTDIR)/$(OUTFILE) $^ $(LIBS) $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~