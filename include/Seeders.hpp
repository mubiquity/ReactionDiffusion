#pragma once
#ifndef REACTIONDIFFUSION2_SEEDERS_HPP
#define REACTIONDIFFUSION2_SEEDERS_HPP

#include "ReactionState.hpp"

template <unsigned int CellDim, unsigned int ChemicalCount>
class AbstractSeeder {
public:
    virtual void seed(ReactionState<CellDim, ChemicalCount> &state)= 0;
    virtual ~AbstractSeeder()= default;
};

/**
 * Seeds a square in the center
 * @tparam CellDim the number of cells along an edge
 * @tparam ChemicalCount the number of chemicals
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class SquareCenterSeed : public AbstractSeeder<CellDim, ChemicalCount> {
public:
    SquareCenterSeed(unsigned int size, const std::array<double, ChemicalCount> &setTo) : size(size/2), setTo(setTo) {};

    void seed(ReactionState<CellDim, ChemicalCount> &state) override {
        for(unsigned int x = CellDim/2 - size; x < (CellDim/2)+size; ++x) {
            for(unsigned int y = CellDim/2 - size; y < (CellDim/2)+size; ++y) {
                state.getConcentration(x, y).conc = setTo;
            }
        }
    }

private:
    const unsigned int size;
    const std::array<double, ChemicalCount> setTo;
};

template <unsigned int CellDim, unsigned int ChemicalCount>
class SpotSeeder : public AbstractSeeder<CellDim, ChemicalCount> {
public:
    SpotSeeder(unsigned int numSpots, unsigned int minSize, unsigned int maxSize, const std::array<double, ChemicalCount> &setTo)
    : numSpots(numSpots), minSize(minSize), maxSize(maxSize), setTo(setTo) {}

    void seed(ReactionState<CellDim, ChemicalCount> &state) override {
        static RandomRange randSpot(0, CellDim);
        static RandomRange randSize(minSize, maxSize);

        for(unsigned int i = 0; i < numSpots; ++i) {
            unsigned int size = randSize()/2;
            unsigned int x = randSpot();
            unsigned int y = randSpot();

            // Start within bounds
            x = size >= x ? 0 : x;
            y = size >= y ? 0 : y;

            unsigned int endX = x + size;
            unsigned int endY = y + size;

            endX = x + size >= CellDim ? CellDim - 1 : endX;
            endY = y + size >= CellDim ? CellDim - 1 : endY;

            for(; x < endX; ++x) {
                for(; y < endY; ++y) {
                    state.getConcentration(x, y).conc = setTo;
                }
            }
        }
    }

private:
    const unsigned int numSpots, minSize, maxSize;
    const std::array<double, ChemicalCount> setTo;
};
#endif //REACTIONDIFFUSION2_SEEDERS_HPP
