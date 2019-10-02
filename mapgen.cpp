#include <SDL2/SDL.h>
#include <cstdio>
#include <cstring>
#include <iostream>

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

	int width = 780;
	int height = 780;

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
  generateMap(width, height, 128, 0.5, 1, 4, 8, map1);

  //saving values in file for analytics


  double max = -1;
  double min = 1;
  for(int i = 0; i < width * height; i++) {
    mapFinal[i] = map1[i]; 
    if(mapFinal[i] > max) max = mapFinal[i];
    if(mapFinal[i] < min) min = mapFinal[i];
  }

  printf("Max: %d\nMin: %d\n", max, min);

  //Worth a try
  //Normalize all points
  double newmax = -1;
  double newmin = -1;
  for(int i = 0; i < width * height; i++) {
    mapFinal[i] = (mapFinal[i] - min) / (max - min);
    if(mapFinal[i] > newmax) newmax = mapFinal[i];
    if(mapFinal[i] < newmin) newmin = mapFinal[i];
  }
  
  printf("NewMax: %d\nNewMin: %d\n", newmax, newmin);

  double maxValue = 1.00f;

  int green = 0x608038;
  int water = 0x0077be;
  int sand = 0xc2b280;
  int snow = 0xfffafa;
  int rocky = 0x353644;
  int swampy = 0x152b12;
  int deep_ocean = 0x01556b;
  
  for(int i = 0; i < width * height; i++) {
    int finalVal = 0;

    if(mapFinal[i] < 0.3 * maxValue) finalVal = deep_ocean;
    else if(mapFinal[i] < 0.45 * maxValue) finalVal = water;
    //else if(perlVal < -0.05) finalVal = swampy;
    else if(mapFinal[i] < 0.4625 * maxValue) finalVal = sand;
    else if(mapFinal[i] < 0.7 * maxValue) finalVal = green;
    else if(mapFinal[i] < 1.0 * maxValue) finalVal = snow;

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
