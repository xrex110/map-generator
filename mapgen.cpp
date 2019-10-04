#include <SDL2/SDL.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ctime>
#include <string>
#include <algorithm>

#include "perlin.hpp"


    //grayscale needs [0, 1]
    //perlVal = (perlVal + 1) / 2;
    //For grayscale
    //int b = (int)(perlVal * 0xFF);
    //int g = b * 0x100;
    //int r = b * 0x10000;
    //finalVal = r + g + b;


using namespace std;

int main(int argc, char** argv) {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL broke :(\n");
		return 0;
	}

	int width = 1200;
	int height = 720;

  //Default map gen param
  double amplitude = 1;
  int octaves = 2;
  double persistence = 0.5; 
  double zVal = 4;

  double inputFreq = 4;

	SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);

  int* pixels = new int[width * height];  //1d mapping of 2d plane
  memset(pixels, 255, sizeof(int) * width * height);

  double* map1 = new double[width * height];  //1d mapping of 2d plane
  double* mapFinal = new double[width * height];  //1d mapping of 2d plane
  memset(map1, 0, sizeof(double) * width * height);
  memset(mapFinal, 0, sizeof(double) * width * height);

  //persistance, amp, freq, numOctaves
  //Lower persistance results in more blobby, less detail maps
  //something that looks like singular continents
  //Higher persistance results in MASSIVE world views, with high
  //detail resolution
  //Amplitude(wrt p) controls the upper bound and lower bounds of the map
  //freq controls the start frequency, start at lower values, and it keeps doubling
  generateMap(width, height, 128, 0.6, 1, 3, 8, map1);

  int space = width * height;

  double max = -1;
  double min = 1;
  for(int i = 0; i < space; i++) {
    mapFinal[i] = map1[i]; 
    if(mapFinal[i] > max) max = mapFinal[i];
    if(mapFinal[i] < min) min = mapFinal[i];
  }

  printf("Max: %f\nMin: %f\n", max, min);

  //Worth a try
  //Normalize all points
  double newmax = -1;
  double newmin = 1;
  for(int i = 0; i < space; i++) {
    mapFinal[i] = (mapFinal[i] - min) / (max - min);
    if(mapFinal[i] > newmax) newmax = mapFinal[i];
    if(mapFinal[i] < newmin) newmin = mapFinal[i];
  }

  printf("After normalization (expected: [0,1])\n");
  printf("NewMax: %f\nNewMin: %f\n", newmax, newmin);

  ////////////////////////////////////////////////////////

  //saving values in file for analytics
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
    return 1;
  }

  for(int i = 0; i < space - 1; i++) {
    fprintf(mapVals, "%.2f,", mapFinal[i]); 
  }
  fprintf(mapVals, "%.2f", mapFinal[space - 1]);
  fclose(mapVals);
  mapVals = NULL;

  ////////////////////////////////////////////////////////

  double maxValue = 1.00f;

  int deep_ocean = 0x01556b;
  int sea_water = 0x006994;
  int coastal_water = 0x0077be;
  int beach_sand = 0xeed6af;
  int inner_sand = 0xd2b55b;
  int lowland_green = 0x608038;    //0x7cfc00;
  int midland_green = 0x4a6904;
  int low_rocky = 0x977c53;
  int rocky = 0x796342;
  int mountainous = 0x9e9e9e;
  int snowy = 0xf0f0ec;
  
  for(int i = 0; i < width * height; i++) {
    int finalVal = 0;

    if(mapFinal[i] < 0.32 * maxValue) finalVal = deep_ocean;
    else if(mapFinal[i] < 0.46 * maxValue) finalVal = sea_water;
    else if(mapFinal[i] < 0.5 * maxValue) finalVal = coastal_water;
    else if(mapFinal[i] < 0.51 * maxValue) finalVal = beach_sand;
    else if(mapFinal[i] < 0.53 * maxValue) finalVal = inner_sand;
    else if(mapFinal[i] < 0.65 * maxValue) finalVal = lowland_green;
    else if(mapFinal[i] < 0.76 * maxValue) finalVal = midland_green;
    else if(mapFinal[i] < 0.79 * maxValue) finalVal = low_rocky;
    else if(mapFinal[i] < 0.82 * maxValue) finalVal = rocky;
    else if(mapFinal[i] < 0.88 * maxValue) finalVal = mountainous;
    else if(mapFinal[i] < 1.0 * maxValue) finalVal = snowy;

    pixels[i] = finalVal;
  }

  SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(int));
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
  
  bool quit = false;

  while(!quit) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT) {
        quit = true;
      }
    }
  }

  delete[] pixels;
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  printf("Program ended\n");
  SDL_Quit();

  return 0;
}
