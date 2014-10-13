$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

PROCESSOR=$(shell uname -p)

CPPFLAGS =  -g -I.

ifeq ($(PROCESSOR), x86_64)
else
  CAMERA_LIB= camera/libcamera.so
  CAMERA_LFLAGS= -lcamera -lopenmaxil -lbcm_host -lrt -L. -L/opt/vc/lib -L./camera
  CPPFLAGS += -DCAMERA
endif

OBJS = main.o  maindelegator.o serialRfid.o serialRfid1356.o web/webservice.o gpio.o



all : $(CAMERA_LIB) main

$(CAMERA_LIB) :
	make -C ./camera

main : ${OBJS} tools/libtool.so $(CAMERA_LIB)
	g++ -o $@ ${OBJS} -L./tools -ltool -lpthread $(CAMERA_LFLAGS)


main.o : maindelegator.h tools/log.h tools/logservice.h
maindelegator.o : maindelegator.h tools/condition.h tools/mutex.h tools/serial.h serialRfid.h serialRfid1356.h tools/log.h tools/logservice.h camera/camerastill.h web/webservice.h
serialRfid.o : serialRfid.h tools/log.h tools/logservice.h
serialRfid1356.o : serialRfid1356.h tools/log.h tools/logservice.h
web/webservice.o : web/webservice.h tools/log.h tools/logservice.h
gpio.o : gpio.h tools/filesystem.h tools/log.h tools/logservice.h
.PHONY : clean
clean :
	-rm *.o main


