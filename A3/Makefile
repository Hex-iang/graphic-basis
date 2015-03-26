#! /usr/bin/make

# Generic Makefile that should work with any program you're going to compile.
# Any complaints should be directed at honghual@sfu.ca

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
# do something Linux-y

INCLUDEDIR=/usr/include/
LIBDIR=/usr/lib

# The source files
SOURCE= scene.cpp image_util.cpp trace.cpp raycast.cpp include/InitShader.cpp

# The compiler 
CC= g++

CFLAGS= -O3 -g -Wall -pedantic -DGL_GLEXT_PROTOTYPES

# The final executable 
EXECUTABLE= raycast

# Linux linkage
LDFLAGS = -lGL -lglut -lGLEW -lXext -lX11 -lm

# Library include
INCLUDEFLAG= -I. -I$(INCLUDEDIR) -Iinclude/
LIBFLAG= -L$(LIBDIR)

# Corresponding object file
OBJECT= $(SOURCE:.cpp=.o)

# under all statement
all: $(OBJECT) depend
	$(CC) $(CFLAGS) $(INCLUDEFLAG) $(LIBFLAG)  $(OBJECT) -o $(EXECUTABLE) $(LDFLAGS)

depend:
	$(CC) -M $(SOURCE) > depend

$(OBJECT):
	$(CC) $(CFLAGS) $(INCLUDEFLAG) -c -o $@ $(@:.o=.cpp)

clean_object:
	rm -f $(OBJECT)

clean:
	rm -f $(OBJECT) depend $(EXECUTABLE)

include depend

endif


ifeq ($(UNAME), Darwin)
# do something Mac-y

INCLUDEDIR=/usr/include/
LIBDIR=/usr/lib

# The source files
SOURCE= scene.cpp image_util.cpp trace.cpp raycast.cpp include/InitShader.cpp

# The compiler 
CC= g++

CFLAGS= -O3 -g -Wall -pedantic -DGL_GLEXT_PROTOTYPES

# The final executable 
EXECUTABLE= raycast

# Linux linkage
LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa

# Library include
INCLUDEFLAG= -I. -I$(INCLUDEDIR) -Iinclude/
LIBFLAG= -L$(LIBDIR)

# Corresponding object file
OBJECT= $(SOURCE:.cpp=.o)

# under all statement
all: $(OBJECT) depend
	$(CC) $(CFLAGS) $(INCLUDEFLAG) $(LIBFLAG)  $(OBJECT) -o $(EXECUTABLE) $(LDFLAGS)

depend:
	$(CC) -M $(SOURCE) > depend

$(OBJECT):
	$(CC) $(CFLAGS) $(INCLUDEFLAG) -c -o $@ $(@:.o=.cpp)

clean_object:
	rm -f $(OBJECT)

clean:
	rm -f $(OBJECT) depend $(EXECUTABLE)

include depend

endif
