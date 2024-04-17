#include "Utility.h"
#include <iostream>

// Load a font from a file. If the file cannot be loaded, print an error
// message and exit the program.
sf::Font loadFont(const std::string &fontPath) {
  sf::Font font;
  if (!font.loadFromFile(fontPath)) {
    std::cerr << "Failed to load font: " << fontPath << std::endl;
    exit(EXIT_FAILURE);
  }
  return font;
}
void drawMovingBall(sf::RenderTarget &target, sf::Clock &clock) {
  // Create a circle shape
  sf::CircleShape circle(50);

  // Get the elapsed time since the clock was started
  sf::Time elapsed = clock.getElapsedTime();

  // Calculate the new position of the circle
  float x = elapsed.asSeconds() * 100.0f; // Move 100 pixels per second
  float y = target.getSize().y / 2.0f;    // Middle of the screen

  // Set the position and color of the circle
  circle.setPosition(x, y);
  circle.setFillColor(sf::Color::Red);

  // Draw the circle
  target.draw(circle);
}

void drawBackround(sf::RenderTarget &target, sf::Color color) {
  sf::RectangleShape background(
      sf::Vector2f(target.getSize().x, target.getSize().y));
  background.setFillColor(color);
  target.draw(background);
}

void drawCellLines(sf::RenderTarget &target, sf::Color color, int cellSize) {
  for (int i = 0; i <= 9; i++) {
    for (int j = 0; j <= 9; j++) {
      sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
      cell.setOutlineThickness(1);
      cell.setOutlineColor(color);
      cell.setPosition(i * cellSize, j * cellSize);
      cell.setFillColor(sf::Color::Transparent);
      target.draw(cell);
    }
  }
}

void drawBoldLines(sf::RenderTarget &target, sf::Color color, int cellSize) {
  for (int i = 0; i <= 9; i++) {
    for (int j = 0; j <= 9; j++) {
      if (i % 3 == 0 && j % 3 == 0) {
        sf::RectangleShape line(
            sf::Vector2f(cellSize * 3, 3)); // Horizontal line
        line.setPosition(i * cellSize, j * cellSize);
        line.setFillColor(color);
        target.draw(line);

        line.setSize(sf::Vector2f(3, cellSize * 3)); // Vertical line
        line.setPosition(i * cellSize, j * cellSize);
        line.setFillColor(color);
        target.draw(line);
      }
    }
  }
}
void drawLines(sf::RenderTarget &target, sf::Color color, int cellSize) {
  // draw cell lines
  drawCellLines(target, color, cellSize);
  drawBoldLines(target, color, cellSize);
}

void drawNumbers(sf::RenderTarget &target,
                 const std::vector<std::vector<char>> &numbers, sf::Font font,
                 sf::Color numberColor, int cellSize) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (numbers[i][j] != '.') {
        sf::Text text;
        text.setFont(font);
        text.setString(numbers[i][j]);
        text.setCharacterSize(24);
        text.setFillColor(numberColor);
        text.setPosition(i * cellSize + 25, j * cellSize + 20);
        target.draw(text); // Draw the numbers
      }
    }
  }
}

// The last lines are drawn outside the grid, this is an easy fix instead of
// of adding offset to every line
void drawClosingLines(sf::RenderTarget &target, sf::Color color, int cellSize) {
  sf::RectangleShape line(sf::Vector2f(cellSize * 9 + 5, 5)); // Horizontal line
  line.setPosition(0, cellSize * 9 - 5);
  line.setFillColor(color);
  target.draw(line);
  line.setSize(sf::Vector2f(5, cellSize * 9 + 5)); // Vertical line
  line.setPosition(cellSize * 9 - 3, 0);
  line.setFillColor(color);
  target.draw(line);
}