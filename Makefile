CC=mpic++
CFLAGS=
LDFLAGS=

# Source
SRCS=$(wildcard src/*.cpp src/**/*.cpp)
INC_DIRS=-Isrc -Ideps/src/inc

# Linux source
OBJS_LINUX=$(wildcard %.cpp,%.o,$(SRCS)) $(wildcard deps/src/obj/linux/*.o)
LIBS_LINUX=$(wildcard deps/src/lib/linux/*)

# Pi source

all:
	mpic++

linux: makeDirectories $(OBJS_LINUX)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_LINUX) \
	      $(LIBS_LINUX) \
	      -o bin/main

%.o: %.c
	$(CC) $(CFLAGS) $(

.PHONY: makeDirectories
makeDirectories:
	@mkdir -p bin
