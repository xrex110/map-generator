all:
	g++ src/perlin.cpp src/mapgen.cpp src/hex.cpp -w -lSDL2 -o ./bin/generate

clean:
	rm bin/generate
