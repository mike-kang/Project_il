$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

MACHINE=$(shell uname -m)

CPPFLAGS =  -g -I. -fPIC 
LIB = libfid.so

ifeq ($(MACHINE), x86_64)
  CPPFLAGS += -DSIMULATOR
else
  CPPFLAGS += -DCAMERA
endif


SRCS =  maindelegator.cpp serialRfid.cpp serialRfid1356.cpp serialRfid900.cpp web/webservice.cpp hardware/gpio.cpp settings.cpp employeeinfomgr.cpp timesheetmgr.cpp
OBJS = $(SRCS:.cpp=.o)

all : $(LIB)
 

$(LIB) : ${OBJS} 
	g++ -shared -o $@ ${OBJS} 
	
depend : $(SRCS)
	g++ -MM $(CPPFLAGS) $^ > $@

.PHONY : clean
clean :
	-rm *.o web/*.o hardware/*.o inih_r29/*.o


-include depend

