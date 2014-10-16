$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

MACHINE=$(shell uname -m)

CPPFLAGS =  -g -I. 

ifeq ($(MACHINE), x86_64)
  CPPFLAGS += -DSIMULATOR
else
  CAMERA_LIB= camera/libcamera.so
  CAMERA_LFLAGS= -lcamera -lopenmaxil -lbcm_host -L. -L/opt/vc/lib -L./camera
  CPPFLAGS += -DCAMERA
endif
INI_LIB= inih_r29/arch/$(MACHINE)/libinih.a
INI_LFLAGS = -L./inih_r29/arch/$(MACHINE) -linih

OBJS = main.o  maindelegator.o serialRfid.o serialRfid1356.o web/webservice.o hardware/gpio.o inih_r29/INIReader.o settings.o employeeinfomgr.o



all : $(CAMERA_LIB) main

$(CAMERA_LIB) :
	make -C ./camera

main : ${OBJS} tools/libtool.so $(CAMERA_LIB) $(INI_LIB)
	g++ -o $@ ${OBJS} -L./tools -ltool -lpthread -lrt $(CAMERA_LFLAGS) $(INI_LFLAGS)

main.o : maindelegator.h tools/log.h tools/logservice.h hardware/switchgpio.h hardware/gpio.h
maindelegator.o : maindelegator.h tools/condition.h tools/mutex.h tools/serial.h serialRfid.h serialRfid1356.h tools/log.h tools/logservice.h camera/camerastill.h web/webservice.h hardware/switchgpio.h hardware/gpio.h settings.h
serialRfid.o : serialRfid.h tools/log.h tools/logservice.h
serialRfid1356.o : serialRfid1356.h tools/log.h tools/logservice.h
web/webservice.o : web/webservice.h tools/log.h tools/logservice.h
hardware/gpio.o : hardware/gpio.h tools/filesystem.h tools/log.h tools/logservice.h
inih_r29/INIReader.o : inih_r29/INIReader.h
settings.o : settings.h tools/log.h tools/logservice.h
employeeinfomgr.o : employeeinfomgr.h web/webservice.h tools/log.h tools/logservice.h  settings.h
.PHONY : clean
clean :
	-rm *.o main


