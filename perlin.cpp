#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "perlin.hpp"

#define LENGTH_OF(X)   ((sizeof(X)/sizeof(0[X])) / ((size_t) (!(sizeof(X) % sizeof(0[X])))))

using namespace std;

static int p[512] = {0}; 
void initPermutations() {
  int perm[256] = {0};
  for(int i = 0; i < LENGTH_OF(perm); i++) {
    perm[i] = i + 1;
  }

  shuffle(perm, LENGTH_OF(perm));  //Shuffle the contents

	for(int i = 0; i < 256; i++) {
		p[256 + i] = p[i] = perm[i];
	}
}

//The generated map is stored in pixels.
void generateMap(int width, int height, double z, double persistance, double amplitude, double frequency, int numberOfOctaves, double* map) {
	memset(map, 255, width * height * sizeof(double));
  initPermutations();

  //enumerate this
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			double dx = ((double) x) / (height);
			double dy = ((double) y) / (width);

      double perlVal = octavePerlin(dx, dy, z/height, persistance, amplitude, frequency, numberOfOctaves);  
      //double perlVal = perlin(dx * frequency, dy * frequency, z * frequency);

      //if(perlVal < 0 || perlVal > 1) {
        //printf("Rogue values detected! RUNNNNN\n");
      //}

			map[y * width + x] = perlVal;
		}
	}
}

double octavePerlin(double x, double y, double z, double persistance, double startAmp, double startFreq, int numOctaves) {
  if(persistance <= 0 || persistance > 1) {
    printf("Persistance needs to be 0 < p < 1");
    return 0;
  }

  double amp = startAmp;
  double freq = startFreq;
  double total = 0;
  double maxValue = 0;

  for(int i = 0; i < numOctaves; i++) {
    total += amp * perlin(x * freq, y * freq, z * freq);
    freq *= 2; //frequency doubles every octave
    maxValue += amp;
    amp *= persistance; //amplitude drops (persistence needs to be 0< persistence < 1)
  }

  //Normalizing data to [0, 1]
  //Inorm = x - min(x) / (max - min)
  return total;  
  //double norm = total + maxValue;
  //norm /= 2 * maxValue;
  //return norm;
}

double perlin(double x, double y, double z) {
     int X = (int)x & 255,
          Y = (int)y & 255,
          Z = (int)z & 255;
      x -= (int) x;         
      y -= (int) y;             
      z -= (int) z;
      double u = fade(x),                                
             v = fade(y),
             w = fade(z);
      int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,
          B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;

  double result = lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                     grad(p[BA  ], x-1, y  , z   )),
                             lerp(u, grad(p[AB  ], x  , y-1, z   ), 
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ), 
                                     grad(p[BA+1], x-1, y  , z-1 )), 
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))));
	//return (result + 1) / 2;	//Translate the [-1, 1] range to [0, 1] range
  return result;

}

double grad(int hash, double x, double y, double z) {
  //int h = hash & 15;
  //double u = h<8 ? x : y,
         //v = h<4 ? y : h==12||h==14 ? x : z;
  //return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
	
  //Riven's improved gradient/distance dotprod function, runs 2x faster on most systems
  switch(hash & 0xF){
    case 0x0: return  x + y;
    case 0x1: return -x + y;
    case 0x2: return  x - y;
    case 0x3: return -x - y;
    case 0x4: return  x + z;
    case 0x5: return -x + z;
    case 0x6: return  x - z;
    case 0x7: return -x - z;
    case 0x8: return  y + z;
    case 0x9: return -y + z;
    case 0xA: return  y - z;
    case 0xB: return -y - z;
    case 0xC: return  y + x;
    case 0xD: return -y + z;
    case 0xE: return  y - x;
    case 0xF: return -y - z;
    default: return 0; // never happens
  }
}

double fade(double t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double lerp(double t, double a, double b) {
  return ((1 - t) * a) + (t * b);
}

void shuffle(int* arr, int length) {
  srand(time(NULL));
  for(int i = 0; i < length * length * length; i++) {
    int a = (rand() % length);
    int b = (rand() % length);
    //Swap the values at indexes a and b
    int t = arr[a];
    arr[a] = arr[b];
    arr[b] = t;
  }

}

