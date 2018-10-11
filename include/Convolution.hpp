#pragma once
#ifndef REACTIONDIFFUSION2_CONVOLUTION_HPP
#define REACTIONDIFFUSION2_CONVOLUTION_HPP

#include <array>

#include "ReactionState.hpp"
#include "CellConcentration.hpp"

/**
 * Abstract convolution functor that essentially just returns the result of some convolution around the centre cell.
 * @tparam CellDim the dimensions in cells of the reaction state
 * @tparam ChemicalCount the number of chemicals
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class AbstractConvolution {
public:
    /**
     * Calls the convolution around the cell at position (x, y) and returns an array of doubles that matches to the result
     * of the convolution for that chemical.
     */
    virtual std::array<double, ChemicalCount> operator()(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state)= 0;
    virtual ~AbstractConvolution()= default;
};


/**
 * A "classic" convolution operates on the center cell of interest and the 8 neighbour cells.
 * Furthermore, all edges share a weight and all corners share a weight. The center also has its own weight.
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class ClassicConvolution : public AbstractConvolution<CellDim, ChemicalCount> {
public:
    ClassicConvolution(double center, double edges, double corners)
            : centerMultiplier(center), edgeMultiplier(edges), cornerMultiplier(corners) {}

    std::array<double, ChemicalCount> operator()(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) override {
        // TODO: Boundary Conditions. Just assumes all indexes valid for now
        std::array<double, ChemicalCount> result = {};

        auto edges = getEdges(x, y, state);
        auto corners = getCorners(x, y, state);
        auto center = state.getConcentration(x, y);

        // Apply the convolution for each chemical
        for(unsigned int chem = 0; chem < ChemicalCount; ++chem) {
            // Do the edges
            for(auto const &edge : edges) {
                result[chem] += edge[chem];
            }
            result[chem] *= edgeMultiplier;

            // Do the corners
            double cornerResult = 0;
            for(auto const &corner : corners) {
                cornerResult += corner[chem];
            }
            result[chem] += (cornerResult * cornerMultiplier);

            // Do the center
            result[chem] += center[chem] * centerMultiplier;
        }

        return result;
    }

protected:
    double centerMultiplier;
    double edgeMultiplier;
    double cornerMultiplier;

    /// Returns edges centered around (x, y) starting from the top and going clockwise
    std::array<CellConcentration<ChemicalCount>, 4> getEdges(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) {
        std::array<CellConcentration<ChemicalCount>, 4> result;

        result[0] = state.getConcentration(x, y - 1);
        result[1] = state.getConcentration(x + 1, y);
        result[2] = state.getConcentration(x, y + 1);
        result[3] = state.getConcentration(x - 1, y);

        return result;
    }

    /// Returns corners centered around (x, y) starting from the top left and going clockwise
    std::array<CellConcentration<ChemicalCount>, 4> getCorners(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) {
        std::array<CellConcentration<ChemicalCount>, 4> result;

        result[0] = state.getConcentration(x - 1, y - 1);
        result[1] = state.getConcentration(x + 1, y - 1);
        result[2] = state.getConcentration(x + 1, y + 1);
        result[3] = state.getConcentration(x - 1, y + 1);

        return result;
    }
};

#endif //REACTIONDIFFUSION2_CONVOLUTION_HPP
