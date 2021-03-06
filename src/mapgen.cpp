#include <SDL2/SDL.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ctime>
#include <string>
#include <algorithm>
#include <cmath>

#include "../include/mapgen.hpp"
#include "../include/perlin.hpp"
#include "../include/hex.hpp"

    //grayscale needs [0, 1]
    //perlVal = (perlVal + 1) / 2;
    //For grayscale
    //int b = (int)(perlVal * 0xFF);
    //int g = b * 0x100;
    //int r = b * 0x10000;
    //finalVal = r + g + b;

using namespace std;

bool saveFlag = false;

void renderMap(int* pixels, int width, int height) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL broke :(\n");
		return;
	}

	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

  SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(int));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

  int sizeGrid;
  hexagon_t* grid = createHexGrid(width, height, 25, &sizeGrid);
  drawHexGrid(renderer, grid, sizeGrid);
  delete[] grid;
  SDL_RenderPresent(renderer);

  //save png
  if(saveFlag) saveMapPNG(renderer, width, height);
  
  bool quit = false;

  while(!quit) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  printf("Program ended\n");
  SDL_Quit();
}

double* createMap(gen_attr attr) {
  int space = attr.width * attr.height; 
  double* map = new double[space];  //1d mapping of 2d plane
  memset(map, 0, sizeof(double) * space);

  //persistance, amp, freq, numOctaves
  //Lower persistance results in more blobby, less detail maps
  //something that looks like singular continents
  //Higher persistance results in MASSIVE world views, with high
  //detail resolution 
  //Amplitude(wrt p) controls the upper bound and lower bounds of the map 
  //freq controls the start frequency, start at lower values, and it keeps doubling 
  
  generateMap(attr, map);

  double max = -1;
  double min = 1;
  for(int i = 0; i < space; i++) {
    if(map[i] > max) max = map[i];
    if(map[i] < min) min = map[i];
  }

  normalizeMap(map, attr.width, attr.height, min, max);
  return map;
}

void normalizeMap(double* map, int width, int height, double min, double max) {
  long space = width * height; 
  for(int i = 0; i < space; i++) {
    map[i] = (map[i] - min) / (max - min);
  }
  printf("Data normalized\n");
}

int* populateBiomes(double* map, int width, int height) {
  long space = width * height;

  int* pixels = new int[space];  //1d mapping of 2d plane
  memset(pixels, 255, sizeof(int) * width * height);

  int deep_ocean = 0x01556b;
  int sea_water = 0x006994;
  int coastal_water = 0x0077be;
  int beach_sand = 0xeed6af;
  int inner_sand = 0xd2b55b;
  int lowland_green = 0x608038;
  int midland_green = 0x4a6904;
  int low_rocky = 0x977c53;
  int rocky = 0x796342;
  int mountainous = 0x9e9e9e;
  int snowy = 0xf0f0ec;

  double maxValue = 1.0f;
  
  for(int i = 0; i < space; i++) {
    int finalVal = 0;

    if(map[i] < 0.32 * maxValue) finalVal = deep_ocean;
    else if(map[i] < 0.46 * maxValue) finalVal = sea_water;
    else if(map[i] < 0.5 * maxValue) finalVal = coastal_water;
    else if(map[i] < 0.51 * maxValue) finalVal = beach_sand;
    else if(map[i] < 0.53 * maxValue) finalVal = inner_sand;
    else if(map[i] < 0.65 * maxValue) finalVal = lowland_green;
    else if(map[i] < 0.76 * maxValue) finalVal = midland_green;
    else if(map[i] < 0.79 * maxValue) finalVal = low_rocky;
    else if(map[i] < 0.82 * maxValue) finalVal = rocky;
    else if(map[i] < 0.88 * maxValue) finalVal = mountainous;
    else if(map[i] < 1.0 * maxValue) finalVal = snowy;

    pixels[i] = finalVal;
  }
  return pixels;
}

int main(int argc, char** argv) {
  //Check for save flag
  //Make into button/hotkey later?
  if (argc == 2) {
    if(!strcmp(argv[1], "save")) {
      saveFlag = true;
    }
    else {
      printf("Invalid argument, please retry\n");
      exit(1);
    }
  }
  
	int width = 620;
	int height = 620;

  gen_attr attr = {
    .persistance = 0.6,
    .lacunarity = 2,
    .frequency = 3,
    .octaves = 8,
    .width = width,
    .height = height,
    .scale = width };

  double* map = createMap(attr);

  if(saveFlag) saveMap(map, width, height);

  int* finalMap = populateBiomes(map, width, height);

  renderMap(finalMap, width, height);

  delete[] map;
  delete[] finalMap;
  return 0;
}

void saveMapPNG(SDL_Renderer* renderer, int width, int height) {
  SDL_Surface* ss = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0); 
  SDL_RenderReadPixels(renderer, NULL, ss->format->format, ss->pixels, ss->pitch);

  time_t cur_time = time(0);
  char* time_as_str = ctime(&cur_time);
  time_as_str[strlen(time_as_str) - 2] = 0;

  string loc("images/Map_");
  string fileName(time_as_str);
  string::iterator end_pos = remove(fileName.begin(), fileName.end(), ' ');
  fileName.erase(end_pos, fileName.end());
  string filetype(".bmp");
  fileName = loc + fileName + filetype;

  SDL_SaveBMP(ss, fileName.c_str());
  SDL_FreeSurface(ss);
}

void saveMap(double points[], int width, int height) {
  long space = width * height;

  time_t cur_time = time(0);
  char* time_as_str = ctime(&cur_time);
  time_as_str[strlen(time_as_str) - 2] = 0;

  string loc("maps/Map_");
  string fileName(time_as_str);
  string::iterator end_pos = remove(fileName.begin(), fileName.end(), ' ');
  fileName.erase(end_pos, fileName.end());
  string filetype(".txt");
  fileName = loc + fileName + filetype;
  
  printf("Writing data to %s\n", fileName.c_str());
  FILE* mapVals = fopen(fileName.c_str(), "w");
  if(!mapVals) {
    printf("File creation failed\n");
    SDL_Quit();
  }

  for(int i = 0; i < space - 1; i++) {
    fprintf(mapVals, "%.2f,", points[i]); 
  }

  fprintf(mapVals, "%.2f", points[space - 1]);
  fclose(mapVals);
  mapVals = NULL;
}

hexagon_t* createHexGrid(int mapWidth, int mapHeight, int size, int* arrSize) {
  point_t temp_c = {0, 0};
  hexagon_t temp = get_hexagon(temp_c, size);
  double widthHex = temp.width;
  double heightHex = temp.height;

  int numWidth = ceil(mapWidth/(widthHex)) + 1;
  int numHeight = ceil(mapHeight/(0.75 * heightHex)) + 1;

  hexagon_t* arr = new hexagon_t[numWidth * numHeight];

  point_t test_c;
  int i = 0;
  for(int row = 0; row < numHeight; row++) {
    for(int col = 0; col < numWidth; col++) {
      if(row % 2 == 0) { //if even row, offset by width
        test_c = {.x = col * widthHex, .y = row * heightHex * 0.75};  
      }
      else  {
        test_c = {.x = (widthHex/2) + col * widthHex, .y = (row * heightHex * 0.75)};
      }
      hexagon_t cur_hex = get_hexagon(test_c, temp.size); 
      arr[i++] = cur_hex; 
    }
  }

  if(i != numWidth * numHeight) printf("Fatal error, segfault prolly\n");
  *arrSize = i;
  return arr;
}

void drawHexGrid(SDL_Renderer* render, hexagon_t* grid, int size) {
  //draw the hexes
  for(int i = 0; i < size; i++) {
    draw_hexagon(render, grid[i]);
  }
}
