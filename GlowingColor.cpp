#include "GlowingColor.h"

sf::Color complementaryPurple = sf::Color(128, 0, 128); // A shade of purple
sf::Color analogousCyan = sf::Color(0, 255, 255);       // A shade of cyan
sf::Color triadicYellow = sf::Color(255, 255, 0);       // A shade of yellow
float GlowingColor::time = 0;
GlowingColor::GlowingColor(sf::Color baseColor) : baseColor(baseColor) {}

sf::Color GlowingColor::getShade() {
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

sf::Color GlowingColor::getBrighterShade() {
  float shadeFactorR = (std::sin(time) + 1) / 2; // Oscillates between 0 and 1
  float shadeFactorG = (std::sin(time + 1) + 1) / 2; // Phase shift of 1
  float shadeFactorB = (std::sin(time * 2) + 1) / 2; // Frequency of 2

  time += 0.001; // Adjust this value to change the speed of the glow

  sf::Color shadedColor;
  shadedColor.r =
      std::min(static_cast<int>(baseColor.r * shadeFactorR + 50), 255);
  shadedColor.g =
      std::min(static_cast<int>(baseColor.g * shadeFactorG + 50), 255);
  shadedColor.b =
      std::min(static_cast<int>(baseColor.b * shadeFactorB + 50), 255);
  shadedColor.a = baseColor.a; // Keep the same alpha value

  return shadedColor;
}
