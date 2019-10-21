#include <SDL2/SDL.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ctime>
#include <string>
#include <algorithm>

#include "../include/mapgen.hpp"
#include "../include/perlin.hpp"


using namespace std;

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
	SDL_RenderClear(renderer); SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  //save png
  saveMapPNG(renderer, width, height);
  
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

  double max = -10000;
  double min = 10000;
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
  int savannah_green = 0x739a6d;
  int desert_yellow = 0xca8d16; 
  int taiga_green = 0x4a4016;
  int tundra = 0xa3bfcb;
  int tropical_green = 0x00755e;
  int rainforest_green = 0x204020;
  int grassland_green = 0x578054;

  gen_attr mois_attr = {
    .persistance = 0.5,
    .lacunarity = 2,
    .frequency = 32,
    .octaves = 1,
    .width = width,
    .height = height,
    .scale = width };

  double* moisture_map = createMap(mois_attr);
  double* temp_map = new double[space];

  double max = -10000;
  double min = 10000;
  for(int i = 0; i < space; i++) {
    temp_map[i] = (map[i] * map[i]) + (0.1 * moisture_map[i]);
    if(temp_map[i] > max) max = temp_map[i];
    else if(temp_map[i] < min) min = temp_map[i];
  }

  printf("temp min: %f, max: %f\n", min, max);

  //normalizeMap(temp_map, width, height, min, max);

  double maxValue = 1.0f;
  
  for(int i = 0; i < space; i++) {
    int finalVal = 0;

    int m_lvl = moistureLevel(temp_map, i);

    //Mappings for biomes based loosely on the 
    //Whittaker biome diagram
    if(map[i] < 0.32 * maxValue) finalVal = deep_ocean;
    else if(map[i] < 0.46 * maxValue) finalVal = sea_water;
    else if(map[i] < 0.5 * maxValue) finalVal = coastal_water;
    else if(map[i] < 0.51 * maxValue) finalVal = beach_sand;
    else if(map[i] < 0.7 * maxValue) {
      switch(m_lvl) {
        case 4: //HIGH TEMP, LOW ELEVATION
          finalVal = tropical_green; 
          break;
        case 3: //MODERATE TO HIGH TEMPERATURE, LOW ELEV
          finalVal = rainforest_green;
          break;
        default: finalVal = grassland_green;  
      }
    }
    else if(map[i] < 0.8) {
      switch(m_lvl) {
        case 4: //High temp
          finalVal = savannah_green;
          break;
        case 3:
          finalVal = grassland_green;
          break;
        case 2:
          finalVal = taiga_green;
          break;
        case 1:
          finalVal = tundra;
          break;
      }
    }
    else if(map[i] < 0.9) {
      switch(m_lvl) {
        case 4: //High temp
          finalVal = desert_yellow;
          break;
        case 3:
          finalVal = savannah_green;
          break;
        case 2:
          finalVal = taiga_green;
          break;
        case 1:
          finalVal = tundra;
          break;
      }
    }
    else if(map[i] <= 1.0) {
      finalVal = mountainous;
    }

    pixels[i] = finalVal;
  }
  return pixels;
}

int moistureLevel(double* map, int i) {
  if(map[i] < 0.25) return 1;  //Low moisture
  else if(map[i] < 0.55) return 2;
  else if(map[i] < 0.9) return 3;
  else if(map[i] <= 1.0) return 4;
  else return -1;
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

int main(int argc, char** argv) {
	int width = 1200;
	int height = 720;

  gen_attr attr = {
    .persistance = 0.6,
    .lacunarity = 2,
    .frequency = 3,
    .octaves = 8,
    .width = width,
    .height = height,
    .scale = 1.5 * width };

  double* map = createMap(attr);

  saveMap(map, width, height);

  int* finalMap = populateBiomes(map, width, height);

  renderMap(finalMap, width, height);

  delete[] map;
  delete[] finalMap;
  return 0;
}
