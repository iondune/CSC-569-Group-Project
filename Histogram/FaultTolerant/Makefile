CXX=mpic++
RM=rm -f
CPPFLAGS=-g -I./Application -I./Utilities
LDFLAGS=-lrt
LDLIBS=

SRCS=$(wildcard *.cpp) $(wildcard Utilities/*.cpp) $(wildcard Application/*.cpp)
HDRS=$(wildcard *.h) $(wildcard Utilities/*.h) $(wildcard Application/*.h)
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

Utilities/DataSet.o: Utilities/DataSet.h
Utilities/MappedFile.o: Utilities/MappedFile.h
Utilities/Timer.o: Utilities/Timer.h
Application/MasterApplication.o: Application/MasterApplication.h
Application/MasterApplication.o: Application/Application.h
Application/MasterApplication.o: Utilities/DataSet.h Utilities/Timer.h
Application/MasterApplication.o: Utilities/MappedFile.h
Application/SlaveApplication.o: Application/SlaveApplication.h
Application/SlaveApplication.o: Application/Application.h Utilities/DataSet.h
Application/SlaveApplication.o: Utilities/Timer.h
Application/Application.o: Utilities/DataSet.h Utilities/Timer.h
Application/MasterApplication.o: Application/Application.h
Application/MasterApplication.o: Utilities/DataSet.h Utilities/Timer.h
Application/SlaveApplication.o: Application/Application.h Utilities/DataSet.h
Application/SlaveApplication.o: Utilities/Timer.h
