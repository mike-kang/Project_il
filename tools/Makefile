$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

OBJS = filesystem.o log.o logservice.o serial.o timer.o network.o date.o base64.o
LIB = libtool.so

CPPFLAGS =  -fPIC -g

all : ${LIB} 

${LIB} : ${OBJS} 
	g++ -shared -o ${LIB} ${OBJS} 
	
filesystem.o : filesystem.h
log.o : log.h logservice.h
logservice.o : logservice.h property.h filesystem.h condition.h queue.h thread.h mutex.h
date.o : date.h
timer.o : timer.h condition.h  mutex.h
network.o : network.h log.h logservice.h
date.o : date.h
base64.o : base64.h

.PHONY : clean
clean :
	-rm *.o ${LIB}


