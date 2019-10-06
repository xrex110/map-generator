#ifndef PERLIN_HPP
#define PERLIN_HPP

//Struct to organize generation attributes
typedef struct perlin_attr {
  double persistance;
  double lacunarity;
  double frequency;
  double octaves;
  int width, height;
  int scale;
} gen_attr;

void initPermutations();
double perlin(double, double, double);
double fade(double);
double lerp(double, double, double);
double grad(int, double, double, double);
double octavePerlin(double, double, double, gen_attr attr);
void generateMap(gen_attr attr, double* pixels) ;
void shuffle(int*, int);

#endif
