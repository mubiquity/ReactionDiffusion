#pragma once
#ifndef REACTIONDIFFUSION2_CELLCONCENTRATION_HPP
#define REACTIONDIFFUSION2_CELLCONCENTRATION_HPP

#include <SFML/Graphics/Color.hpp>

#include <array>
#include <iterator>
#include <algorithm>

#include "Util.hpp"
#include "Random.hpp"

/**
 * Defines the concentration of each chemical at a particular cell in the grid.
 * For more efficiency this could probably follow a SOA pattern instead of the AOS pattern but this is far
 * simpler to work with and does increase flexibility.
 * @tparam ChemicalCount the number of chemicals to simulate
 */
template <unsigned int ChemicalCount>
struct CellConcentration {
    std::array<double, ChemicalCount> conc = {};
    CellConcentration()= default;
    explicit CellConcentration(const std::array<double, ChemicalCount> &conc) : conc(conc) {}

    /// Make and return a new random CellConcentration
    static CellConcentration makeRandom() {
        static RandomFloatingRange rand(0, 1);

        std::array<double, ChemicalCount> conc;

        for(auto &val : conc) {
            val = rand();
        }

        return CellConcentration(conc);
    }

    /// Converts the cell concentration to an SFML colour based on the concentration of each chemical
    sf::Color toColor() const {
        unsigned long total = 0;
        for(unsigned int i = 0; i < ChemicalCount; ++i) {
            unsigned int hue = ((i + 4) * 41); // + 4 for nice blue
            sf::Color col = colorFromHSL(hue, 0.70, 0.5);

            double multiplier = i; // incorporating i can give cool effects
            col.a = static_cast<sf::Uint8>(conc[i] * col.a * multiplier);
            total += col.toInteger();
        }
        return sf::Color(static_cast<sf::Uint32>(total/ChemicalCount));

        auto index = std::distance(conc.begin(), std::max_element(conc.begin(), conc.end()));

        // Uses HSL to get a nice bright colour with a (reasonably) reliably different hue for each chemical
        // + 6 because I like blue.
        // 41 for separation and because it is prime so going over 360 should still be relatively unique after modulo
        unsigned int hue = ((static_cast<unsigned int>(index) + 6) * 41);
        return colorFromHSL(hue, 0.70, 0.5);
    }

    double operator[](unsigned int index) const {
        return conc[index];
    }

    double &operator[](unsigned int index) {
        return conc[index];
    }

    // ====== Iteration ======

    using iterator = typename std::array<double, ChemicalCount>::iterator;
    using const_iterator = typename std::array<double, ChemicalCount>::const_iterator;

    iterator begin() { return conc.begin(); }

    iterator end() { return conc.end(); }

    const_iterator begin() const { return conc.begin(); }

    const_iterator end() const { return conc.end(); }

    const_iterator cbegin() const { return conc.cbegin(); }

    const_iterator cend() const { return conc.cend(); }
};

#endif //REACTIONDIFFUSION2_CELLCONCENTRATION_HPP
