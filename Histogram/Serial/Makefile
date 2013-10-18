CXX=g++
RM=rm -f
CPPFLAGS=-g
LDFLAGS=
LDLIBS=

SRCS=Serial.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: Serial

Serial: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .

include .depend
