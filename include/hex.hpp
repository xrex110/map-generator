#ifndef HEX_HPP
#define HEX_HPP

typedef struct pt {
  double x;
  double y;
} point_t;

typedef struct hex {
  point_t center;
  int size;
  double width, height;
  point_t points[6];
} hexagon_t;

point_t hex_corner(point_t center, int size, int i);
hexagon_t get_hexagon(point_t center, int size);
void draw_hexagon(SDL_Renderer* render, point_t center, int size);
void draw_hexagon(SDL_Renderer* render, hexagon_t hex);

#endif
