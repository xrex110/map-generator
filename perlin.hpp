void initPermutations();
double perlin(double, double, double);
double fade(double);
double lerp(double, double, double);
double grad(int, double, double, double);
double octavePerlin(double, double, double, double, double, double, int);
void generateMap(int width, int height, double z, double persistance, double amplitude, double frequency, int numberOfOctaves, double* pixels) ;
void shuffle(int*, int);
