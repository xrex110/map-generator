all:
	g++ perlin.cpp mapgen.cpp -w -lSDL2 -o generate

clean:
	rm generate
