all: server.o thread.o server_func.o packet.o
	g++ server.o thread.o server_func.o packet.o -lpthread -o server
server_func.o:
	g++ -c server_func.c -o server_func.o
packet.o:
	g++ -c packet.c -o packet.o
thread.o: server_func.o packet.o
	g++ -c thread.c -o thread.o
server.o: thread.o server_func.o packet.o
	g++ -lpthread -c server.c -o server.o
