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
class AbstractSpotSeeder : public AbstractSeeder<CellDim, ChemicalCount> {
public:
    AbstractSpotSeeder(unsigned int numSpots, unsigned int minSize, unsigned int maxSize)
    : numSpots(numSpots), minSize(minSize), maxSize(maxSize) {}

    virtual void setCell(ReactionState<CellDim, ChemicalCount> &state, unsigned int x, unsigned int y)=0;

protected:
    const unsigned int numSpots, minSize, maxSize;

private:
    void seed(ReactionState<CellDim, ChemicalCount> &state) override {
        setCells(state);
    }

    void setCells(ReactionState<CellDim, ChemicalCount> &state) {
        static RandomRange randSpot(0, CellDim);
        static RandomRange randSize(minSize, maxSize);

        for(unsigned int i = 0; i < numSpots; ++i) {
            unsigned int size = randSize()/2;
            unsigned int x = randSpot();
            unsigned int y = randSpot();

            // Start within bounds
            unsigned int startX = size >= x ? 0 : x - size;
            unsigned int startY = size >= y ? 0 : y - size;

            unsigned int endX = x + size >= CellDim ? CellDim - 1 : x + size;
            unsigned int endY = y + size >= CellDim ? CellDim - 1 : y + size;

            for(unsigned int xIndex = startX; xIndex < endX; ++xIndex) {
                for(unsigned int yIndex = startY; yIndex < endY; ++yIndex) {
                    setCell(state, xIndex, yIndex);
                }
            }
        }
    }
};

template <unsigned int CellDim, unsigned int ChemicalCount>
class SpotSeeder : public AbstractSpotSeeder<CellDim, ChemicalCount> {
public:
    SpotSeeder(unsigned int numSpots, unsigned int minSize, unsigned int maxSize, const std::array<double, ChemicalCount> &setTo)
    : AbstractSpotSeeder<CellDim, ChemicalCount>(numSpots, minSize, maxSize), setTo(setTo) {}

    void setCell(ReactionState<CellDim, ChemicalCount> &state, unsigned int x, unsigned int y) override {
        state.getConcentration(x, y).conc = setTo;
    }

private:
    const std::array<double, ChemicalCount> setTo;
};

template <unsigned int CellDim, unsigned int ChemicalCount>
class RandomSeeder : public AbstractSeeder<CellDim, ChemicalCount> {
public:
    void seed(ReactionState<CellDim, ChemicalCount> &state) override {
        for(unsigned int x = 0; x < CellDim; ++x) {
            for(unsigned int y = 0; y < CellDim; ++y) {
                state.getConcentration(x, y) = CellConcentration<ChemicalCount>::makeRandom();
            }
        }
    }
};

template <unsigned int CellDim, unsigned int ChemicalCount>
class RandomSpotSeeder : public AbstractSpotSeeder<CellDim, ChemicalCount> {
public:
    RandomSpotSeeder(unsigned int numSpots, unsigned int minSize, unsigned int maxSize)
            : AbstractSpotSeeder<CellDim, ChemicalCount>(numSpots, minSize, maxSize) {}

    void setCell(ReactionState<CellDim, ChemicalCount> &state, unsigned int x, unsigned int y) override {
        state.getConcentration(x, y) = CellConcentration<ChemicalCount>::makeRandom();
    }

};
#endif //REACTIONDIFFUSION2_SEEDERS_HPP
