CXX=mpic++
RM=rm -f
CPPFLAGS=-g
LDFLAGS=-lrt
LDLIBS=

SRCS=$(wildcard *.cpp)
HDRS=$(wildcard *.h)
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

DataSet.o: DataSet.h
FaultTolerant.o: Application.h DataSet.h Timer.h MasterApplication.h
FaultTolerant.o: SlaveApplication.h
MappedFile.o: MappedFile.h
MasterApplication.o: MasterApplication.h Application.h DataSet.h Timer.h
MasterApplication.o: MappedFile.h
SlaveApplication.o: SlaveApplication.h Application.h DataSet.h Timer.h
Timer.o: Timer.h
Application.o: DataSet.h Timer.h
MasterApplication.o: Application.h DataSet.h Timer.h
SlaveApplication.o: Application.h DataSet.h Timer.h
