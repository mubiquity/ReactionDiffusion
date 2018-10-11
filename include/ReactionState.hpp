#pragma once
#ifndef REACTIONDIFFUSION2_REACTIONSTATE_HPP
#define REACTIONDIFFUSION2_REACTIONSTATE_HPP

#include <SFML/Graphics/Color.hpp>

#include <memory>
#include <array>
#include <iostream>

#include "CellConcentration.hpp"

/**
 * The current state of each cell in the cell grid.
 * @tparam CellDim the number of cells along each side
 * @tparam ChemicalCount the number of chemicals to simulate
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class ReactionState {
public:
    static constexpr unsigned int NumCells = CellDim * CellDim;

    ReactionState()= default;

    /// Construct a new randomised reaction state
    explicit ReactionState(std::array<double, ChemicalCount> initialAmounts) {
        for(unsigned int i = 0; i < NumCells; ++i) {
            cells[i] = CellConcentration<ChemicalCount>(initialAmounts);
        }
    }

    inline CellConcentration<ChemicalCount> getConcentration(unsigned int x, unsigned int y) const {
        return cells[(y * CellDim) + x];
    }

    inline CellConcentration<ChemicalCount> &getConcentration(unsigned int x, unsigned int y) {
        return cells[(y * CellDim) + x];
    }

    /// Returns a vector of colours that can be used to draw the reaction state
    sf::Uint8 *getColoring() {
        //unsigned int i = 0;
        // TODO: Not entirely convinced this one is worth it
        #pragma omp parallel for schedule(static)
        for(unsigned int index = 0; index < NumCells; ++index) {
            sf::Color col = cells[index].toColor();
            unsigned int i = index * 4;
            coloring[i + 0] = col.r;
            coloring[i + 1] = col.g;
            coloring[i + 2] = col.b;
            coloring[i + 3] = col.a;
        }

        return &coloring[0];
    }

    // Non-copyable
    ReactionState(ReactionState &other)= delete;
    ReactionState& operator=(ReactionState &source)= delete;

    // Move semantics
    ReactionState(ReactionState &&source) noexcept : cells(std::move(source.cells)) {}
    ReactionState& operator=(ReactionState &&source) noexcept {
        std::swap(cells, source.cells);
        return *this;
    }

    ~ReactionState() {
        delete [] cells;
        delete [] coloring;
    };

private:
    CellConcentration<ChemicalCount> *cells = new CellConcentration<ChemicalCount>[NumCells];
    sf::Uint8 *coloring = new sf::Uint8[NumCells*4];
};

#endif //REACTIONDIFFUSION2_REACTIONSTATE_HPP
