#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "ReactionDiffusion.hpp"
#include "Convolution.hpp"
#include "ReactionModel.hpp"

#include <iostream>

constexpr unsigned int WINDOW_SIZE = 600;
constexpr unsigned int CHEMICALS = 2;

int main() {

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE, 32), "Gray-Scott Reaction Diffusion");
    // Create a new model that fits to the window size with a half cell gap around the edges
    auto convolution = std::unique_ptr<AbstractConvolution<WINDOW_SIZE, CHEMICALS>>(new ClassicConvolution<WINDOW_SIZE, CHEMICALS>(-1, 0.2, 0.05, BoundaryCondition::Wrap));
    //auto seeder = std::unique_ptr<AbstractSeeder<WINDOW_SIZE, CHEMICALS>>(new SquareCenterSeed<WINDOW_SIZE, CHEMICALS>(40, {0, 1}));
    auto seeder = std::unique_ptr<AbstractSeeder<WINDOW_SIZE, CHEMICALS>>(new SpotSeeder<WINDOW_SIZE, CHEMICALS>(20, 4, 15, {0, 1}));
    //auto seeder = std::unique_ptr<AbstractSeeder<WINDOW_SIZE, CHEMICALS>>(new RandomSeeder<WINDOW_SIZE, CHEMICALS>());
    //auto seeder = std::unique_ptr<AbstractSeeder<WINDOW_SIZE, CHEMICALS>>(new RandomSpotSeeder<WINDOW_SIZE, CHEMICALS>(20, 10, 20));
    auto reactionModel = std::unique_ptr<AbstractReactionModel<CHEMICALS>>(GrayScottModel::coral());
    //auto reactionModel = std::unique_ptr<AbstractReactionModel<CHEMICALS>>(new PredatorPreyModel(2./3., 20./3., 1, 1));

    ReactionDiffusion<WINDOW_SIZE, CHEMICALS> model(std::move(convolution), std::move(seeder), std::move(reactionModel));

    // Run the main application loop
    sf::Clock clock;
    bool paused = true;
    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            // Handle close events
            if(event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }else if(event.type == sf::Event::KeyPressed) {
                // Exit on escape pressed
                if(event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    return 0;
                } else if(event.key.code == sf::Keyboard::Space) {
                    paused = !paused;
                }
            }

            model.onEvent(event);
        }

        // Update the model
        if(!paused) {
            for(int i = 0; i < 5; ++i) {
                model.update(clock.restart());
            }
        }

        // Draw the result
        window.clear(sf::Color::Black);
        model.prepareDraw();
        window.draw(model);
        window.display();
    }
}