all:
	g++ -o bin/game-server main.cpp src/*.cpp -lSDL2 -lSDL2_net
	./bin/game-server
just-build:
	g++ -o bin/game-server main.cpp src/*.cpp -lSDL2 -lSDL2_net
run:
	./bin/game-server