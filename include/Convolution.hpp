#pragma once
#ifndef REACTIONDIFFUSION2_CONVOLUTION_HPP
#define REACTIONDIFFUSION2_CONVOLUTION_HPP

#include <array>

#include "ReactionState.hpp"
#include "CellConcentration.hpp"

enum BoundaryCondition {
    Wrap,
    Restrict,
};

/**
 * Abstract convolution functor that essentially just returns the result of some convolution around the centre cell.
 * @tparam CellDim the dimensions in cells of the reaction state
 * @tparam ChemicalCount the number of chemicals
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class AbstractConvolution {
public:
    explicit AbstractConvolution(BoundaryCondition boundaryCondition) : boundaryCondition(boundaryCondition) {};
    /**
     * Calls the convolution around the cell at position (x, y) and returns an array of doubles that matches to the result
     * of the convolution for that chemical.
     */
    virtual std::array<double, ChemicalCount> operator()(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state)= 0;
    virtual ~AbstractConvolution()= default;

protected:
    const BoundaryCondition boundaryCondition = BoundaryCondition::Wrap;
};


/**
 * A "classic" convolution operates on the center cell of interest and the 8 neighbour cells.
 * Furthermore, all edges share a weight and all corners share a weight. The center also has its own weight.
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class ClassicConvolution : public AbstractConvolution<CellDim, ChemicalCount> {
public:
    ClassicConvolution(double center, double edges, double corners, BoundaryCondition boundaryCondition=BoundaryCondition::Wrap)
            : AbstractConvolution<CellDim, ChemicalCount>(boundaryCondition), centerMultiplier(center), edgeMultiplier(edges), cornerMultiplier(corners) {}

    std::array<double, ChemicalCount> operator()(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) override {
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
    std::vector<CellConcentration<ChemicalCount>> getEdges(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) {
        std::vector<CellConcentration<ChemicalCount>> result;
        result.reserve(4);

        switch (this->boundaryCondition) {
            case Wrap:
                result.push_back(state.getConcentration(x, y == 0 ? CellDim - 1 : y - 1)); // Above
                result.push_back(state.getConcentration(x >= CellDim - 1 ? 0 : x + 1, y)); // Right
                result.push_back(state.getConcentration(x, y >= CellDim - 1 ? 0 : y + 1)); // Below
                result.push_back(state.getConcentration(x == 0 ? CellDim - 1 : x - 1, y)); // Left
                break;
            case Restrict:
                if(y != 0) {
                    result.push_back(state.getConcentration(x, y - 1));
                }
                if(x < CellDim - 1) {
                    result.push_back(state.getConcentration(x + 1, y));
                }
                if(y < CellDim - 1) {
                    result.push_back(state.getConcentration(x, y + 1));
                }
                if(x != 0) {
                    result.push_back(state.getConcentration(x - 1, y));
                }
                break;
        }

        return result;
    }

    /// Returns corners centered around (x, y) starting from the top left and going clockwise
    std::vector<CellConcentration<ChemicalCount>> getCorners(unsigned int x, unsigned int y, const ReactionState<CellDim, ChemicalCount> &state) {
        std::vector<CellConcentration<ChemicalCount>> result;
        result.reserve(4);

        switch (this->boundaryCondition) {
            case Wrap:
            {
                unsigned int takeX = x == 0 ? CellDim - 1 : x - 1;
                unsigned int takeY = y == 0 ? CellDim - 1 : y - 1;
                unsigned int plusX = x >= CellDim - 1 ? 0 : x + 1;
                unsigned int plusY = y >= CellDim - 1 ? 0 : y + 1;

                result.push_back(state.getConcentration(takeX, takeY)); // Top-Left
                result.push_back(state.getConcentration(plusX, takeY)); // Top-Right
                result.push_back(state.getConcentration(plusX, plusY)); // Bottom-Right
                result.push_back(state.getConcentration(takeX, plusY)); // Bottom-Left
                break;
            }
            case Restrict:
                if(x != 0) {
                    if(y != 0) {
                        result.push_back(state.getConcentration(x - 1, y - 1)); // Top-Left
                    }
                    if(y < CellDim - 1) {
                        result.push_back(state.getConcentration(x - 1, y + 1)); // Bottom-Left
                    }
                }
                if(x < CellDim - 1) {
                    if(y != 0) {
                        result.push_back(state.getConcentration(x + 1, y - 1)); // Top-Right
                    }
                    if(y < CellDim - 1) {
                        result.push_back(state.getConcentration(x + 1, y + 1)); // Bottom-Right
                    }
                }
                break;
        }

        return result;
    }
};

#endif //REACTIONDIFFUSION2_CONVOLUTION_HPP
