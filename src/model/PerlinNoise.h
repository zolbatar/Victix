#pragma once

#include <vector>

class PerlinNoise {
private:
    std::vector<int> permutation;
    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y);

public:
    PerlinNoise(unsigned int seed = 0);
    double noise(double x, double y);
    double noise(double x, double y, double freq, int depth);
};
