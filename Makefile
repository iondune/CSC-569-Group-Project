CC=mpic++
CFLAGS=
LDFLAGS=

# Source
SRCS=$(wildcard src/*.cpp src/**/*.cpp)

# Linux source
OBJS_LINUX=$(wildcard %.cpp,%.o,$(SRCS)) $(wildcard deps/src/obj/linux/*.o)
LIBS_LINUX=$(wildcard deps/src/lib/linux/*)

# Pi source

linux: makeDirectories $(OBJS_LINUX)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_LINUX) \
	      $(LIBS_LINUX) \
	      -o bin/main

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

foo:
	mpic++ -I. src/main.cpp lib/vector.cpp lib/strutil.c deps/src/obj/linux/* deps/src/lib/linux/libmrmpi_linux.a

.PHONY: makeDirectories
makeDirectories:
	@mkdir -p bin
