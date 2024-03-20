#include "Sudoku.h"
#include "GlowingColor.h"
#include <iostream>
#include <random>
# include <fstream>

const int cellSize = 64;

namespace Utility {
sf::Color complementaryPurple(128, 0, 128); // A shade of purple
sf::Color analogousCyan(0, 255, 255);       // A shade of cyan
sf::Color triadicYellow(255, 255, 0);       // A shade of yellow

// Load a font from a file. If the file cannot be loaded, print an error
// message and exit the program.
sf::Font loadFont(const std::string &filepath) {
  sf::Font font;
  if (!font.loadFromFile(filepath)) {
    std::cerr << "Failed to load font: " << filepath << std::endl;
    exit(EXIT_FAILURE);
  }
  return font;
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
                 const std::vector<std::vector<int>> &numbers, sf::Font font,
                 sf::Color numberColor, int cellSize) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (numbers[i][j] != 0) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(numbers[i][j]));
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
} // namespace Utility

// Constructor for the Sudoku class. Initializes the Sudoku grid and generates
// a puzzle.
Sudoku::Sudoku() {
  initGrid();
  generatePuzzle("");
  font = Utility::loadFont("../Lacquer-Regular.ttf");
}

Sudoku::Sudoku(const std::string &difficulty, const std::string &filename) {
  initGrid();
  font = Utility::loadFont("../Lacquer-Regular.ttf");
  if (filename.empty()) {
    generatePuzzle(difficulty);
  } else {
    // parse the file chars into a 2d vector of chars
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Failed to open file: " << filename << std::endl;
      exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 9; i++) {
      std::string line;
      std::getline(file, line);
      for (int j = 0; j < 9; j++) {
        numbers[i][j] = line[j] - '0';
      }
    }
  }
}

void Sudoku::setDarkTheme() {
  backgroundColor = sf::Color(0, 0, 0); // Black
  numberColor = sf::Color::Green;
  gridColor = Utility::analogousCyan;
}

// Get the current state of the Sudoku board.
const std::vector<std::vector<char>> Sudoku::getBoard() {
  std::vector<std::vector<char>> board;
  for (int i = 0; i < 9; i++) {
    std::vector<char> row;
    for (int j = 0; j < 9; j++) {
      if (numbers[i][j] == 0) {
        row.push_back('.');
      } else {
        row.push_back(numbers[i][j] + '0');
      }
    }
    board.push_back(row);
  }
  return board;
}

// Initialize the Sudoku grid.
void Sudoku::initGrid() {
  for (int i = 0; i < 9; i++) {
    std::vector<sf::RectangleShape> row;
    for (int j = 0; j < 9; j++) {
      sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
      cell.setOutlineThickness(1);
      cell.setOutlineColor(gridColor);
      cell.setPosition(i * cellSize, j * cellSize);
      row.push_back(cell);
    }
    grid.push_back(row);
  }

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      cells.push_back(grid[i][j]);
    }
  }

  numbers = std::vector<std::vector<int>>(9, std::vector<int>(9, 0));
}

void Sudoku::saveScreenshot(sf::RenderTarget &target) const {
  sf::RenderTexture renderTexture;
  renderTexture.create(target.getSize().x, target.getSize().y);

  // Check if target is a RenderWindow
  if (sf::RenderWindow *window = dynamic_cast<sf::RenderWindow *>(&target)) {
    sf::Texture texture;
    texture.create(window->getSize().x, window->getSize().y);
    texture.update(*window);
    renderTexture.draw(sf::Sprite(texture));
  }
  // Check if target is a RenderTexture
  else if (sf::RenderTexture *texture =
               dynamic_cast<sf::RenderTexture *>(&target)) {
    renderTexture.draw(sf::Sprite(texture->getTexture()));
  }

  renderTexture.display();
  sf::Image board_image = renderTexture.getTexture().copyToImage();
  std::time_t result = std::time(nullptr);
  std::string filename = "board" + std::to_string(result) + ".png";
  if (!board_image.saveToFile(filename)) {
    std::cerr << "Failed to save" << std::endl;
  }
}
// Draw the Sudoku grid and numbers.
void Sudoku::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  auto glowingColor = GlowingColor(Utility::analogousCyan).getShade();
  auto glowingBrighterColor =
      GlowingColor(Utility::analogousCyan).getBrighterShade();
  Utility::drawBackround(target, backgroundColor);
  Utility::drawLines(target, glowingColor, cellSize);
  Utility::drawNumbers(target, numbers, font, numberColor, cellSize);
  Utility::drawClosingLines(target, glowingBrighterColor, cellSize);
  saveScreenshot(target);
}

void Sudoku::fillGrid() {
  std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(nums.begin(), nums.end(), g);

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      numbers[i][j] =
          nums[(i * 3 + i / 3 + j) % 9]; // Fill the grid with numbers in a way
                                         // that forms a valid Sudoku solution
    }
  }
}

void Sudoku::generatePuzzle(const std::string &difficulty) {
  // Start with a valid, filled Sudoku grid
  fillGrid();
  int difficultyLevel = 40;
  if (difficulty != "") {
    difficultyLevel = std::stoi(difficulty);
  }

  // Randomly remove numbers from the grid
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 8);
  for (int i = 0; i < difficultyLevel; i++) {
    int row = dis(gen);
    int col = dis(gen);
    numbers[row][col] = 0;
    grid[row][col].setFillColor(backgroundColor);
  }
}

// Handle a click event on the Sudoku grid.
void Sudoku::handleClick(int x, int y, int mouseButton) {
  int cellX = x / cellSize;
  int cellY = y / cellSize;
  if (mouseButton == sf::Mouse::Right) {
    numbers[cellX][cellY] = 0;
  } else {
    numbers[cellX][cellY] = (numbers[cellX][cellY] % 9) + 1;
  }
}

// Set the current state of the Sudoku board.
void Sudoku::setBoard(const std::vector<std::vector<char>> &newBoard) {
  numbers = std::vector<std::vector<int>>(9, std::vector<int>(9, 0));
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (newBoard[i][j] != '.') {
        numbers[i][j] = newBoard[i][j] - '0';
      }
    }
  }

  // show on the grid
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      std::cout << newBoard[i][j] << " ";
    }
    std::cout << std::endl;
  }
}