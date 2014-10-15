
all : test_network
LFLAGS = -L.. -ltool -lpthread -lrt
test_network : ../network.o test_network.o
	g++ -o $@ $^ $(LFLAGS)
	

