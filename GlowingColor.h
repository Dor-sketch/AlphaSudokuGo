#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
extern sf::Color complementaryPurple;
extern sf::Color analogousCyan;
extern sf::Color triadicYellow;
class GlowingColor {
private:
  sf::Color baseColor;
  static float time;

public:
  GlowingColor(sf::Color baseColor);

  sf::Color getShade();

  sf::Color getBrighterShade();
};
// Initialize the static member
