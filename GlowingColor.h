# pragma once
# include <SFML/Graphics.hpp>
# include <cmath>

class GlowingColor {
private:
    sf::Color baseColor;
    static float time;

public:
    GlowingColor(sf::Color baseColor) : baseColor(baseColor) {}

    sf::Color getShade() {
        float shadeFactorR = (std::sin(time) + 1) / 2; // Oscillates between 0 and 1
        float shadeFactorG = (std::sin(time + 1) + 1) / 2; // Phase shift of 1
        float shadeFactorB = (std::sin(time * 2) + 1) / 2; // Frequency of 2

        time += 0.001; // Adjust this value to change the speed of the glow

        sf::Color shadedColor;
        shadedColor.r = std::min(static_cast<int>(baseColor.r * shadeFactorR), 255);
        shadedColor.g = std::min(static_cast<int>(baseColor.g * shadeFactorG), 255);
        shadedColor.b = std::min(static_cast<int>(baseColor.b * shadeFactorB), 255);
        shadedColor.a = baseColor.a; // Keep the same alpha value

        return shadedColor;
    }

    sf::Color getBrighterShade() {
        float shadeFactorR = (std::sin(time) + 1) / 2; // Oscillates between 0 and 1
        float shadeFactorG = (std::sin(time + 1) + 1) / 2; // Phase shift of 1
        float shadeFactorB = (std::sin(time * 2) + 1) / 2; // Frequency of 2

        time += 0.001; // Adjust this value to change the speed of the glow

        sf::Color shadedColor;
        shadedColor.r = std::min(static_cast<int>(baseColor.r * shadeFactorR + 50), 255);
        shadedColor.g = std::min(static_cast<int>(baseColor.g * shadeFactorG + 50), 255);
        shadedColor.b = std::min(static_cast<int>(baseColor.b * shadeFactorB + 50), 255);
        shadedColor.a = baseColor.a; // Keep the same alpha value

        return shadedColor;
    }
};

// Initialize the static member
float GlowingColor::time = 0;