all: client.o client_func.o packet.o
	g++ client.o client_func.o packet.o -o client
client.o:
	g++ -c client.c -o client.o
client_func.o:
	g++ -c client_func.c -o client_func.o
packet.o:
	g++ -c packet.c -o packet.o
