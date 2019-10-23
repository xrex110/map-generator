#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include "perlin.hpp"
#include "hex.hpp"

double* createMap(gen_attr attr);
void normalizeMap(double*, int, int, double, double);
int* populateBiomes(double* map, int, int);
void renderMap(int*, int, int);
void saveMapPNG(SDL_Renderer*, int, int);
void saveMap(double[], int, int);

hexagon_t* createHexGrid(int mapWidth, int mapHeight, int size, int* arrSize);
void drawHexGrid(SDL_Renderer* render, hexagon_t* grid, int size);

#endif
