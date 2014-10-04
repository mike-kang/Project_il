$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

PROCESSOR=$(shell uname -p)

CPPFLAGS =  -g

ifeq ($(PROCESSOR), x86_64)
else
  CAMERA_LIB= libraspi.a
  CAMERA_LFLAGS= -lraspi -lmmal_core -lmmal_util -lmmal_vc_client -lvcos -lbcm_host -lrt -L. -L/opt/vc/lib
  CPPFLAGS += -DCAMERA
endif

OBJS = main.o



all : main

main : main.o  maindelegator.o serialRfid.o serialRfid1356.o webservice.o gpio.o tools/libtool.so 
	g++ -o $@ main.o maindelegator.o serialRfid.o serialRfid1356.o webservice.o gpio.o -L./tools -ltool -lpthread $(CAMERA_LFLAGS)


main.o : maindelegator.h tools/log.h tools/logservice.h
maindelegator.o : maindelegator.h tools/condition.h tools/mutex.h tools/serial.h serialRfid.h serialRfid1356.h tools/log.h tools/logservice.h libRaspiStill.h $(CAMERA_LIB)
serialRfid.o : serialRfid.h tools/log.h tools/logservice.h
serialRfid1356.o : serialRfid1356.h tools/log.h tools/logservice.h
webservice.o : webservice.h tools/log.h tools/logservice.h
gpio.o : gpio.h tools/filesystem.h tools/log.h tools/logservice.h
.PHONY : clean
clean :
	-rm *.o main


