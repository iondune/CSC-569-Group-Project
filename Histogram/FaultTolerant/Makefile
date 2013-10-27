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
FaultTolerant.o: hrt.h DataSet.h MappedFile.h
hrt.o: hrt.h
MappedFile.o: MappedFile.h
