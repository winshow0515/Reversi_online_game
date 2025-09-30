CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

all: server client

server: server.cpp game.hpp
	$(CXX) $(CXXFLAGS) server.cpp -o server

client: client.cpp game.hpp
	$(CXX) $(CXXFLAGS) client.cpp -o client

clean:
	rm -f server client

.PHONY: all clean