
all : test_network test_media test_base64

LFLAGS = -L.. -ltool -lpthread -lrt

test_network : ../network.o test_network.o
	g++ -o $@ $^ $(LFLAGS)
	
test_media : test_media.o
	g++ -o $@ $^

test_base64 : test_base64.o
	g++ -o $@ $^ $(LFLAGS)

test_network.o : ../network.h
test_media.o : ../media.h
test_base64.o : ../base64.h

.PHONY : clean
clean:
	rm *.o test_network test_wavplayer
