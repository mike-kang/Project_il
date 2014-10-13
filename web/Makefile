$(warning os: $(shell uname -s))
$(info current dir is ${CURDIR})


OBJS = main.o webservice.o

EXE = webservice
all : ${EXE}

CPPFLAGS =   -g -I..

${EXE} :  ${OBJS}
	g++  -o ${EXE}  ${OBJS} -L../tools -ltool -lrt -lpthread
	

.PHONY : clean
clean :
	-rm *.o  ${EXE}


