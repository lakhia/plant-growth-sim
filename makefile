# This Makefile makes the `plant-grow' program, from its source and
# header files.
#
# To use this Makefile, type:
#
#        make BOX=<machinetype> 
#
# where	<machinetype> is either "osx" or "linux"

# First set up variables we'll use when making things
PROG = plant-grow
SOURCES = plant.cpp lowlevel.cpp bitmap.cpp
INC = -I/usr/X11R6/include/
C++ = g++
# CFLAGS = -c -O3 -mcpu=pentium3 -march=pentium3 -mfpmath=sse -fno-enforce-eh-specs -ffast-math -fomit-frame-pointer
CFLAGS = -Wall -c -g -Wno-deprecated
OBJ_DIR = build
SRC_DIR = .
OBJS = build/plant.o build/lowlevel.o build/bitmap.o

# LDLIBS varies based on the machine type
ifeq ($(BOX), linux)
LDLIBS = -L/usr/X11R6/lib/ -lglut -lGLU -lm
else
LDLIBS = -framework GLUT -framework OpenGL
endif


# Finally, build the program
$(PROG): $(OBJS)
	$(C++) $(OBJS) $(LDLIBS) -o $(PROG)
build/bitmap.o: bitmap.cpp bitmap.h
	@mkdir -p $(OBJ_DIR)
	$(C++) $(CFLAGS) $(INC) bitmap.cpp -o build/bitmap.o
build/lowlevel.o: lowlevel.cpp lowlevel.h
	@mkdir -p $(OBJ_DIR)
	$(C++) $(CFLAGS) $(INC) lowlevel.cpp -o build/lowlevel.o
build/plant.o: plant.cpp plant.h lowlevel.h bitmap.h
	@mkdir -p $(OBJ_DIR)
	$(C++) $(CFLAGS) $(INC) plant.cpp -o build/plant.o

# Rule to clean
clean:
	rm -fr $(OBJ_DIR) $(PROG)
