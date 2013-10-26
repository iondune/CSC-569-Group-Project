CC=mpic++
CFLAGS=-Wall -O3
LDFLAGS=-Wall
INC_DIRS=-I.

###############################################################################
# Source                                                                      #
###############################################################################
MAIN=main.o

SRCS=$(wildcard src/*.cpp src/**/*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

OBJS_LINUX=$(OBJS) $(wildcard deps/src/obj/linux/*.o)
LIBS_LINUX=$(wildcard deps/src/lib/linux/*)

OBJS_PI=$(OBJS) $(wildcard deps/src/obj/mpicc/*.o)
LIBS_PI=$(wildcard deps/src/lib/mpicc/*)

###############################################################################
# Test                                                                        #
###############################################################################
TEST_MAIN=test/main.o

TEST_SRCS=$(SRCS) $(wildcard test/*.cpp test/**/*.cpp)
TEST_OBJS=$(patsubst %.cpp,%.o,$(TEST_SRCS))

TEST_OBJS_LINUX=$(TEST_OBJS) $(wildcard deps/src/obj/linux/*.o)
TEST_LIBS_LINUX=$(wildcard deps/src/lib/linux/*) -lgtest

TEST_OBJS_PI=$(TEST_OBJS) $(wildcard deps/src/obj/mpicc/*.o)
TEST_LIBS_PI=$(wildcard deps/src/lib/mpicc/*)

###############################################################################

test-linux: $(TEST_OBJS)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(TEST_OBJS) \
	      $(TEST_LIBS_LINUX) \
	      -o bin/test-linux

linux: makeDirectories $(OBJS_LINUX) $(MAIN)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_LINUX) \
	      $(MAIN) \
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
