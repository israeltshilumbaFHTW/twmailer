all: server client 

server: server.cpp
	g++ -std=c++14 -Wall -Werror -o server server.cpp
client: client.cpp
	g++ -std=c++14 -Wall -Werror -o client client.cpp
start:
	 ./server
clear:
	rm -f client server 