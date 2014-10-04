$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})

OBJS = property.o  filesystem.o log.o logservice.o serial.o
LIB = libtool.so

CPPFLAGS =  -fPIC -g

all : ${LIB} 

${LIB} : ${OBJS} 
	g++ -shared -o ${LIB} ${OBJS} 
	
property.o : property.h
filesystem.o : filesystem.h
log.o : log.h logservice.h
logservice.o : logservice.h property.h filesystem.h condition.h queue.h thread.h mutex.h
date.o : date.h
.PHONY : clean
clean :
	-rm *.o ${LIB}


