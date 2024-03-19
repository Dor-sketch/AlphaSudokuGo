#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Sudoku : public sf::Drawable {
public:
  Sudoku();
  void initGrid();
  void handleClick(int x, int y, int mouseButton);
  std::vector<std::vector<char>> getBoard();
  void setBoard(std::vector<std::vector<char>> &newBoard);

protected:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

private:
  sf::Font font;   // Declare a font for the numbers
  void fillGrid(); // Fill the grid with numbers
  const int cellSize = 64;
  void generatePuzzle();
  std::vector<std::vector<sf::RectangleShape>>
      grid; // Declare grid as a 2D vector of sf::RectangleShape
  std::vector<sf::RectangleShape>
      cells; // Declare cells as a vector of sf::RectangleShape
  std::vector<std::vector<int>> numbers; // Declare numbers as a 2D vector
};