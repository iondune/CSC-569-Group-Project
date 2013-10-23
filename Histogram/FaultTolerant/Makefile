CXX=g++
RM=rm -f
CPPFLAGS=-g
LDFLAGS=-lrt
LDLIBS=

SRCS=FaultTolerant.cpp hrt.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: FaultTolerant

FaultTolerant: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

depend: .depend

.depend:
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .

include .depend
