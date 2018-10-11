#pragma once
#ifndef REACTIONDIFFUSION_RANDOM_HPP
#define REACTIONDIFFUSION_RANDOM_HPP

#include <random>

/**
 * Calling returns a random unsigned integer in the range from a to b non inclusive
 */
class RandomRange {
public:
    RandomRange(unsigned int a, unsigned int b) : rd(), mt(rd()), dist(a, b - 1) {}

    unsigned int operator()() {
        return dist(mt);
    }

private:
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<unsigned int> dist;
};

class RandomFloatingRange {
public:
    RandomFloatingRange(double a, double b) : rd(), mt(rd()), dist(a, b) {}

    double operator()() {
        return dist(mt);
    }

private:
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_real_distribution<double> dist;
};

#endif //REACTIONDIFFUSION_RANDOM_HPP
