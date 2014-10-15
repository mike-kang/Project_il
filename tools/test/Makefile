
all : test_network test_media

LFLAGS = -L.. -ltool -lpthread -lrt

test_network : ../network.o test_network.o
	g++ -o $@ $^ $(LFLAGS)
	
test_media : test_media.o
	g++ -o $@ $^

test_network.o : ../network.h
test_media.o : ../media.h


.PHONY : clean
clean:
	rm *.o test_network test_wavplayer