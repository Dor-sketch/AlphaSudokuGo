#pragma once

#include <SFML/Graphics.hpp>

void drawBoldLines(sf::RenderTarget &target, sf::Color color, int thickness);
void drawLines(sf::RenderTarget &target, sf::Color color, int thickness);
void drawCellLines(sf::RenderTarget &target, sf::Color color, int thickness);
void drawMovingBall(sf::RenderTarget &target, sf::Clock &clock);
void drawClosingLines(sf::RenderTarget &target, sf::Color color, int thickness);
sf::Font loadFont(const std::string &fontPath);
// void drawSmoke(sf::RenderTarget &target, sf::Clock &clock);
void drawBackround(sf::RenderTarget &target, sf::Color color);
void drawNumbers(sf::RenderTarget &target,
                 const std::vector<std::vector<char>> &numbers, sf::Font font,
                 sf::Color color, int fontSize);
