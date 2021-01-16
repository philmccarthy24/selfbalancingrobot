#############################################################
#
# Compiles named cpp files. Handles source files in nested
# directories using VPATH trick.
#
# (update _OBJ to control cpps built, VPATH to add additional
# source directories, and CXXFLAGS to add additional include 
# directories). 
#
# Puts obj files in obj/.
# "debug" target outputs to bin/debug/, default ("release") target
# outputs to bin/release/.
#
# Munged together from a lot of places, it's quite clunky
# but works.
# Phil McCarthy Jan 2021
#
#############################################################
.DEFAULT_GOAL := release

CXX := g++
CXXFLAGS := -I./ -Ilib/I2Cdev -Ilib/MPU6050

VPATH=lib/I2Cdev:lib/MPU6050

ODIR=obj
LDIR=
LIBS=-lbcm2835 -lm
OUTFILE = sbrcontrollerd

_OBJ = main.o I2Cdev.o MPU6050.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp
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