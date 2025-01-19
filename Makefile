all:
	g++ -o bin/game-server main.cpp src/*.cpp -Ilibs/include -Llibs/lib -lSDL2 -lSDL2_net
	./bin/game-server
just-build:
	g++ -o bin/game-server main.cpp src/*.cpp -Ilibs/include -Llibs/lib -lSDL2 -lSDL2_net
release:
	g++ -o bin/game-server main.cpp src/*.cpp -Ilibs/include -Llibs/lib -lSDL2 -lSDL2_net -spdlog
run:
	./bin/game-server