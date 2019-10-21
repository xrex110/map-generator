#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include "perlin.hpp"

double* createMap(gen_attr attr);
void normalizeMap(double*, int, int, double, double);
int* populateBiomes(double* map, int, int);
int moistureLevel(double* map, int i);
void renderMap(int*, int, int);
void saveMapPNG(SDL_Renderer*, int, int);
void saveMap(double[], int, int);

#endif
