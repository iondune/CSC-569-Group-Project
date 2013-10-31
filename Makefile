CC=mpic++
CFLAGS=-Wall -O3
LDFLAGS=-Wall
INC_DIRS=-I. -I./deps/src/inc

###############################################################################
# Source                                                                      #
###############################################################################
MAIN=./Histogram/Parallel/MrMain.o ./Histogram/Parallel/vector.o ./Histogram/Parallel/strutil.o ./Histogram/Parallel/fileOutput.o

SRCS=$(wildcard src/*.cpp src/**/*.cpp)
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

OBJS_LINUX=$(OBJS) $(wildcard deps/src/obj/linux/*.o)
LIBS_LINUX=$(wildcard deps/src/lib/linux/*)

OBJS_PI=$(OBJS) $(wildcard ../mrmpi-23Oct13/src/Obj_mpicc/*.o)
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

linux: makeDirectories $(OBJS_LINUX) $(MAIN)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_LINUX) \
	      $(MAIN) \
	      $(LIBS_LINUX) \
	      -o bin/main-linux

test-linux: $(TEST_OBJS)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(TEST_OBJS) \
	      $(TEST_LIBS_LINUX) \
	      -o bin/test-linux

pi: makeDirectories $(OBJS_PI) $(MAIN)
	$(CC) $(LDFLAGS) \
	      $(INC_DIRS) \
	      $(OBJS_PI) \
	      $(MAIN) \
	      $(LIBS_PI) \
	      -o bin/main-pi

%.o: %.cpp
	$(CC) $(INC_DIRS) $(CFLAGS) -c $< -o $@

.PHONY: makeDirectories
makeDirectories:
	@mkdir -p bin
