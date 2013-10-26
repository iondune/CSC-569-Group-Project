CC=mpic++
CFLAGS=
LDFLAGS=

# Source
SRCS=$(wildcard src/*.cpp src/**/*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))
INC_DIRS=-I.

# Linux source
OBJS_LINUX=$(OBJS) $(wildcard deps/src/obj/linux/*.o)
LIBS_LINUX=$(wildcard deps/src/lib/linux/*)

# Pi source
OBJS_PI=$(OBJS) $(wildcard deps/src/obj/mpicc/*.o)
LIBS_PI=$(wildcard deps/src/lib/mpicc/*)


linux: makeDirectories $(OBJS_LINUX)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_LINUX) \
	      $(LIBS_LINUX) \
	      -o bin/main-linux

pi: makeDirectories $(OBJS_PI)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_PI) \
	      $(LIBS_PI) \
	      -o bin/main-pi

%.o: %.cpp
	$(CC) -I. $(CFLAGS) -c $< -o $@

.PHONY: makeDirectories
makeDirectories:
	@mkdir -p bin
