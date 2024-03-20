#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Sudoku : public sf::Drawable {
public:
  Sudoku();
  Sudoku(const std::vector<std::vector<char>> &board);
  void handleClick(int x, int y, int mouseButton);
  void setBoard(const std::vector<std::vector<char>> &newBoard);
  const std::vector<std::vector<char>> getBoard();

protected:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
  void drawBoldLines(sf::RenderTexture &renderTexture);
  void drawClosingLines(sf::RenderTexture &renderTexture);
  void drawText(sf::RenderTexture &renderTexture);
  void drawGridLines(sf::RenderTexture &renderTexture);
  void saveScreenshot(sf::RenderTexture &RenderTexture);
  void initGrid();
  void fillGrid(); // Fill the grid with numbers
  void generatePuzzle();

  const int cellSize = 64;
  sf::Font font; // Declare a font for the numbers
  std::vector<std::vector<sf::RectangleShape>>
      grid; // Declare grid as a 2D vector of sf::RectangleShape
  std::vector<sf::RectangleShape>
      cells; // Declare cells as a vector of sf::RectangleShape
  std::vector<std::vector<int>> numbers; // Declare numbers as a 2D vector
};