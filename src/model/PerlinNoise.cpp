#include <cmath>
#include <vector>
#include <random>
#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(unsigned int seed) {
    permutation.resize(256);
    std::iota(permutation.begin(), permutation.end(), 0);
    std::default_random_engine engine(seed);
    std::shuffle(permutation.begin(), permutation.end(), engine);
    permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

double PerlinNoise::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) {
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y) {
    int h = hash & 3;
    double u = h < 2 ? x : y;
    double v = h < 2 ? y : x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double PerlinNoise::noise(double x, double y) {
    int X = (int) floor(x) & 255;
    int Y = (int) floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    double u = fade(x);
    double v = fade(y);

    int a = permutation[X] + Y;
    int aa = permutation[a];
    int ab = permutation[a + 1];
    int b = permutation[X + 1] + Y;
    int ba = permutation[b];
    int bb = permutation[b + 1];

    return lerp(v, lerp(u, grad(permutation[aa], x, y), grad(permutation[ba], x - 1, y)),
                lerp(u, grad(permutation[ab], x, y - 1), grad(permutation[bb], x - 1, y - 1)));
}

double PerlinNoise::noise(double x, double y, double freq, int depth) {
    double xa = x * freq;
    double ya = y * freq;
    double amp = 1.0;
    double fin = 0.0;
    double di = 0.0;
    for (int i = 0; i < depth; i++) {
        di += 1.0 * amp;
        fin += noise(xa, ya) * amp;
        amp /= 2.0;
        xa *= 2.0;
        ya *= 2.0;
    }
    return fin / di;
}