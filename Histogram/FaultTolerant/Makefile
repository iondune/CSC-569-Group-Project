CXX=mpic++
RM=rm -f
CPPFLAGS=-O3 -I./Utilities
LDFLAGS=-O3
LDLIBS=

SRCS=$(wildcard *.cpp) $(wildcard */*.cpp)
HDRS=$(wildcard *.h) $(wildcard */*.h)
OBJS=$(subst .cpp,.o,$(SRCS))

all: FaultTolerant

FaultTolerant: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

depend:
	@echo Regenerating local dependencies.
	@makedepend -Y $(SRCS) $(HDRS) -s "# Dependencies" > /dev/null 2>&1
	@rm Makefile.bak

clean:
	$(RM) $(OBJS)

remake: clean all

# Dependencies

FaultTolerant.o: Application/Application.h
FaultTolerant.o: Application/NetworkBenchmarkMasterApplication.h
FaultTolerant.o: Application/Application.h
FaultTolerant.o: Application/NetworkBenchmarkSlaveApplication.h
FaultTolerant.o: Application/FaultTolerantMasterApplication.h
FaultTolerant.o: Application/FaultTolerantSlaveApplication.h
FaultTolerant.o: Application/ParallelMasterApplication.h
FaultTolerant.o: Application/ParallelSlaveApplication.h
Utilities/DataSet.o: Utilities/DataSet.h
Utilities/MappedFile.o: Utilities/MappedFile.h
Utilities/Timer.o: Utilities/Timer.h
Application/Application.o: Utilities/DataSet.h Utilities/Timer.h
Application/FaultTolerantMasterApplication.o: Application/Application.h
Application/FaultTolerantMasterApplication.o: Utilities/MappedFile.h
Application/FaultTolerantSlaveApplication.o: Application/Application.h
Application/NetworkBenchmarkMasterApplication.o: Application/Application.h
Application/NetworkBenchmarkMasterApplication.o: Utilities/MappedFile.h
Application/NetworkBenchmarkSlaveApplication.o: Application/Application.h
Application/ParallelMasterApplication.o: Application/Application.h
Application/ParallelMasterApplication.o: Utilities/MappedFile.h
Application/ParallelSlaveApplication.o: Application/Application.h
