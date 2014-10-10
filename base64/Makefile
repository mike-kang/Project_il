$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})


OBJS = main.o base64.o
#LIB = ../src/libb64.a
EXE = base64
all : ${EXE}

CPPFLAGS =   -g -I../include

${EXE} :  ${OBJS}
	g++  -o ${EXE}  ${OBJS}
	

.PHONY : clean
clean :
	-rm *.o  ${EXE}


