#ifndef REACTIONDIFFUSION2_REACTIONDIFFUSION_HPP
#define REACTIONDIFFUSION2_REACTIONDIFFUSION_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <omp.h>

#include "ReactionState.hpp"
#include "ReactionModel.hpp"
#include "Convolution.hpp"
#include "Seeders.hpp"

/**
 * Controls the whole simulation.
 * @tparam CellDim the dimensions of the cell grid
 * @tparam CellSize the size of each cell in the cell grid
 * @tparam ChemicalCount the number of chemicals to simulate
 */
template <unsigned int CellDim, unsigned int ChemicalCount>
class ReactionDiffusion : public sf::Drawable {
public:

    explicit ReactionDiffusion(std::unique_ptr<AbstractConvolution<CellDim, ChemicalCount>> convolution,
            std::unique_ptr<AbstractSeeder<CellDim, ChemicalCount>> seeder,
            std::unique_ptr<AbstractReactionModel<ChemicalCount>> reactionModel)
            : convolution(std::move(convolution)), reactionModel(std::move(reactionModel))
    {
        seedReaction(std::move(seeder));
    }

    // TODO: Abstract this
    void seedReaction(std::unique_ptr<AbstractSeeder<CellDim, ChemicalCount>> seeder) {
        reactionState = ReactionState<CellDim, ChemicalCount>(std::array<double, ChemicalCount>{1, 0});

        seeder->seed(reactionState);
    }

    void onEvent(sf::Event) {

    }

    void update(sf::Time) {
        static ReactionState<CellDim, ChemicalCount> nextState = ReactionState<CellDim, ChemicalCount>(std::array<double, ChemicalCount> {1, 0});

        unsigned int x, y;
        #pragma omp parallel for collapse(2) private(x, y)
        for(x = 0; x < CellDim; ++x) {
            for(y = 0; y < CellDim; ++y) {
                std::array<double, ChemicalCount> convRes = (*convolution)(x, y, reactionState);

                const CellConcentration<ChemicalCount> conc = reactionState.getConcentration(x, y);

                nextState.getConcentration(x, y).conc = reactionModel->update(conc, convRes);
            }
        }

        reactionState = std::move(nextState);
    }

    void prepareDraw() {
        image.create(CellDim, CellDim, reactionState.getColoring());
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
        static sf::Texture tex;
        static sf::Sprite sprite;
        tex.loadFromImage(image);
        sprite.setTexture(tex);
        target.draw(sprite, states);
    }

private:
    ReactionState<CellDim, ChemicalCount> reactionState;
    sf::Image image;
    std::unique_ptr<AbstractConvolution<CellDim, ChemicalCount>> convolution;
    std::unique_ptr<AbstractReactionModel<ChemicalCount>> reactionModel;

};

#endif //REACTIONDIFFUSION2_REACTIONDIFFUSION_HPP
