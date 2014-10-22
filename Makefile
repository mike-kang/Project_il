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

OBJS =  maindelegator.o serialRfid.o serialRfid1356.o web/webservice.o hardware/gpio.o settings.o employeeinfomgr.o

all : $(LIB)
 

$(LIB) : ${OBJS} 
	g++ -shared -o $@ ${OBJS} 
	
main.o : maindelegator.h tools/log.h tools/logservice.h hardware/switchgpio.h hardware/gpio.h
maindelegator.o : maindelegator.h tools/condition.h tools/mutex.h tools/serial.h serialRfid.h serialRfid1356.h tools/log.h tools/logservice.h camera/camerastill.h web/webservice.h hardware/switchgpio.h hardware/gpio.h settings.h
serialRfid.o : serialRfid.h tools/log.h tools/logservice.h
serialRfid1356.o : serialRfid1356.h tools/log.h tools/logservice.h
web/webservice.o : web/webservice.h tools/log.h tools/logservice.h
hardware/gpio.o : hardware/gpio.h tools/filesystem.h tools/log.h tools/logservice.h
settings.o : settings.h tools/log.h tools/logservice.h
employeeinfomgr.o : employeeinfomgr.h web/webservice.h tools/log.h tools/logservice.h  settings.h
.PHONY : clean
clean :
	-rm *.o web/*.o hardware/*.o inih_r29/*.o


