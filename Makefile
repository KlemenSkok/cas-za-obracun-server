all:
	g++ -o bin/game-server main.cpp src/*.cpp -Ilibs/include -Llibs/lib -lSDL2 -lSDL2_net -lspdlog
	./bin/game-server
just-build:
	g++ -o bin/game-server main.cpp src/*.cpp -Ilibs/include -Llibs/lib -lSDL2 -lSDL2_net -lspdlog
run:
	./bin/game-server