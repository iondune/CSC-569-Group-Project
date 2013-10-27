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

# Dependencies

FaultTolerant.o: hrt.h
hrt.o: hrt.h
