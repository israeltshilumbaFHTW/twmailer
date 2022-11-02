all: server client 

server: server.cpp
	g++ -std=c++14 -Wall -Werror -o compiledObject/server server.cpp
client: client.cpp
	g++ -std=c++14 -Wall -Werror -o compiledObject/client client.cpp
test:
	g++ -std=c++14 -Wall -Werror -o compiledObject/test test.cpp
start:
	 ./server
clear:
	rm -f client server 