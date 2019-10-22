#include <SDL2/SDL.h>
#include <cmath>

#include "../include/hex.hpp"

#define PI_VAL 3.1415926535897
#define SQRT_3 1.732

point_t hex_corner(point_t center, int size, int i) {
  int angle_degrees = 60 * i - 30;
  double angle_radians = (PI_VAL/180) * angle_degrees;
  point_t corner_coord = {.x = center.x + (size * cos(angle_radians)), .y = center.y + (size * sin(angle_radians))};
  return corner_coord;
}

hexagon_t get_hexagon(point_t center, int size) {
  hexagon_t new_hex = {.center = center, .size = size};

  for(int i = 0; i < 6; i++) {
    new_hex.points[i] = hex_corner(center, size, i);
  }

  //For pointy. We flip these for flat-topped
  new_hex.height = 2 * size;
  new_hex.width = SQRT_3 * size;

  return new_hex;
}

void draw_hexagon(SDL_Renderer* render, point_t center, int size) {
  hexagon_t hex = get_hexagon(center, size);
  draw_hexagon(render, hex);
}

void draw_hexagon(SDL_Renderer* render, hexagon_t hex) {
  for(int i = 0; i < 5; i++) {
    SDL_RenderDrawLine(render, hex.points[i].x, hex.points[i].y, hex.points[i+1].x, hex.points[i+1].y);
  }
  SDL_RenderDrawLine(render, hex.points[5].x, hex.points[5].y, hex.points[0].x, hex.points[0].y);

  //To confirm bounds are proper...
  //SDL_RenderDrawLine(render, hex.center.x, hex.center.y, hex.center.x + hex.size, hex.center.y);
  //SDL_RenderDrawLine(render, hex.center.x, hex.center.y, hex.center.x, hex.center.y - hex.size);
}
