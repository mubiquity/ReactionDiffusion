/**
 * Basic utility functions
 */
#pragma once
#ifndef REACTIONDIFFUSION_UTIL_HPP
#define REACTIONDIFFUSION_UTIL_HPP

#include <cmath>

#include <SFML/Graphics/Color.hpp>

double constrainZeroOne(double in) {
    return std::min(1.0, std::max(0.0, in));
}

float hueToRGB(float v1, float v2, float vH) {
    if (vH < 0)
        vH += 1;

    if (vH > 1)
        vH -= 1;

    if ((6 * vH) < 1)
        return (v1 + (v2 - v1) * 6 * vH);

    if ((2 * vH) < 1)
        return v2;

    if ((3 * vH) < 2)
        return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

    return v1;
}

/**
 * Convert HSL to an SFML RGB Color
 *
 * @param hue the hue of the colour - between 0 and 360 inclusive
 * @param saturation the saturation - between 0 and 1
 * @param luminosity the luminosity - between 0 and 1
 */
sf::Color colorFromHSL(unsigned int hue, float saturation, float luminosity) {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    if (saturation == 0)
    {
        r = g = b = static_cast<unsigned char>(luminosity * 255);
    }
    else
    {
        float v1, v2;
        float convertedHue = static_cast<float>(hue) / 360.0F;

        v2 = (luminosity < 0.5) ? (luminosity * (1 + saturation)) : ((luminosity + saturation) - (luminosity * saturation));
        v1 = 2 * luminosity - v2;

        r = static_cast<unsigned char>(255 * hueToRGB(v1, v2, convertedHue + (1.0f / 3)));
        g = static_cast<unsigned char>(255 * hueToRGB(v1, v2, convertedHue));
        b = static_cast<unsigned char>(255 * hueToRGB(v1, v2, convertedHue - (1.0f / 3)));
    }

    return sf::Color(r, g, b);
}

#endif //REACTIONDIFFUSION_UTIL_HPP
