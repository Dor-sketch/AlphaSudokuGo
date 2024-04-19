#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "Particals.h"


class Sudoku : public sf::Drawable {
public:
  Sudoku(const int cellSize);
  // Sudoku(const std::string &difficulty, const std::string &filename);
  Sudoku(const std::vector<std::vector<char>> &board, const int cellSize);
  void handleClick(int x, int y, int mouseButton);
  void setBoard(const std::vector<std::vector<char>> &newBoard);
  const std::vector<std::vector<char>> getBoard();
  void saveScreenshot(sf::RenderTarget &target) const;
  void setDarkTheme();
  void setLightTheme();
  unsigned int particleCount = 10;
protected:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
private:
 int cellSize = 50;
  sf::Color numberColor = sf::Color::Black;
  sf::Color backgroundColor = sf::Color::White;
  sf::Color gridColor = sf::Color::Blue;
  void initGrid();
  void fillGrid(); // Fill the grid with numbers
  void generatePuzzle(const std::string &difficulty);

  sf::Font font; // Declare a font for the numbers
  std::vector<std::vector<sf::RectangleShape>>
      grid; // Declare grid as a 2D vector of sf::RectangleShape
  std::vector<sf::RectangleShape>
      cells; // Declare cells as a vector of sf::RectangleShape
  std::vector<std::vector<char>> numbers; // Declare numbers as a 2D vector
};