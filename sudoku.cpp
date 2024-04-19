#include "Sudoku.h"
#include "GlowingColor.h"
#include "Parser.h"
#include "Utility.h"
#include <fstream>
#include <iostream>
#include <random>
const char *fontPath = "../VT323-Regular.ttf";
#include "Particals.h"


// Constructor for the Sudoku class. Initializes the Sudoku grid and generates
// a puzzle.
Sudoku::Sudoku(const int cellSize) : cellSize(cellSize) {
  initGrid();
  generatePuzzle("");
  font = loadFont(fontPath);
}

Sudoku::Sudoku(const std::vector<std::vector<char>> &board, const int cellSize)
    : cellSize(cellSize) {
  initGrid();
  setBoard(board);
  font = loadFont(fontPath);
}


void Sudoku::setDarkTheme() {
  backgroundColor = sf::Color(0, 0, 0); // Black
  numberColor = sf::Color::Green;
  gridColor = complementaryPurple;
}

void Sudoku::setLightTheme() {
  backgroundColor = sf::Color::White;
  numberColor = sf::Color::Black;
  gridColor = sf::Color::Blue;
}

// Get the current state of the Sudoku board.
const std::vector<std::vector<char>> Sudoku::getBoard() { return numbers; }

// Initialize the Sudoku grid.
void Sudoku::initGrid() {
std::cout<<cellSize<<std::endl;
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

  numbers = std::vector<std::vector<char>>(9, std::vector<char>(9, '.'));
}

void Sudoku::saveScreenshot(sf::RenderTarget &target) const {
  sf::RenderTexture renderTexture;
  renderTexture.create(target.getSize().x, target.getSize().y);

  // Check if target is a RenderWindow
  if (sf::RenderWindow *window = dynamic_cast<sf::RenderWindow *>(&target)) {
    sf::Texture texture;
    // set size of the texture to the size of the window
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
  auto glowingColor = GlowingColor(analogousCyan).getShade();
  auto glowingBrighterColor = GlowingColor(analogousCyan).getBrighterShade();
  drawBackround(target, backgroundColor);
  static sf::Clock clock; // Create a static sf::Clock object
                          // drawMovingFigure(target, clock);
  // drawMovingBackground(target, clock, particleCount, numbers, effect_type);
  // drawAllAround(target, clock, cellSize, numbers);
  drawLines(target, glowingColor, cellSize);
  drawNumbers(target, numbers, font, numberColor, cellSize);
  drawClosingLines(target, glowingBrighterColor, cellSize);
  // saveScreenshot(target);
}

void Sudoku::fillGrid() {
  std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(nums.begin(), nums.end(), g);

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      char c = (i * 3 + i / 3 + j) % 9 + '1';
      numbers[i][j] = c; // Fill the grid with numbers in a way
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
    numbers[row][col] = '.';
    grid[row][col].setFillColor(backgroundColor);
  }
}

// Handle a click event on the Sudoku grid.
void Sudoku::handleClick(int x, int y, int mouseButton) {
  int cellX = x / cellSize;
  int cellY = y / cellSize;
  if (mouseButton == sf::Mouse::Right) {
    numbers[cellX][cellY] = '.';
  } else {
    char c = numbers[cellX][cellY];
    if (c == '.') {
      c = '1';
    } else {

      c = (c - '0' + 1) % 10 + '0';
    }
    numbers[cellX][cellY] = c;
  }
}

// Set the current state of the Sudoku board.
void Sudoku::setBoard(const std::vector<std::vector<char>> &newBoard) {
  //        // if board is already empty, generate a new random puzzle
  if (newBoard ==
      std::vector<std::vector<char>>(9, std::vector<char>(9, '.'))) {
    if (numbers != newBoard) {
      numbers = newBoard;
    } else {
      generatePuzzle("");
    }
  } else {
    numbers = newBoard;
  }
}