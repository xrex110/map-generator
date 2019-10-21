#ifndef MAPGEN_HPP
#define MAPGEN_HPP

#include "perlin.hpp"

typedef struct pt {
  double x;
  double y;
} point_t;

double* createMap(gen_attr attr);
void normalizeMap(double*, int, int, double, double);
int* populateBiomes(double* map, int, int);
void renderMap(int*, int, int);
void saveMapPNG(SDL_Renderer*, int, int);
void saveMap(double[], int, int);

point_t hex_corner(point_t center, int size, int i);

#endif
