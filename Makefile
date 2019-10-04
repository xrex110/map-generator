all:
	g++ perlin.cpp mapgen.cpp -w -lSDL2 -o ./bin/generate

clean:
	rm bin/generate
