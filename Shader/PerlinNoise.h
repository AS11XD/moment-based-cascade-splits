#ifndef PERNLIN_H
#define PERNLIN_H

#include <vector>

//from: https://github.com/sol-prog/Perlin_Noise

class PerlinNoise
{
	std::vector<int> p;
public:
	PerlinNoise();
	~PerlinNoise();
	PerlinNoise(unsigned int seed);
	double noise(double x, double y, double z);
private:
	double fade(double t);
	double lerp(double t, double a, double b);
	double grad(int hash, double x, double y, double z);
};

#endif // !PERNLIN_H