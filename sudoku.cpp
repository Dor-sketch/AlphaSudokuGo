#include "sudoku.h"
#include "solver.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

// Load a font from a file. If the file cannot be loaded, print an error message
// and exit the program.
sf::Font load_font(const std::string &filepath) {
  sf::Font font;
  if (!font.loadFromFile(filepath)) {
    std::cerr << "Failed to load font: " << filepath << std::endl;
    exit(EXIT_FAILURE);
  }
  return font;
}
// Constructor for the Sudoku class. Initializes the Sudoku grid and generates a
// puzzle.
Sudoku::Sudoku() {
  std::cout << "Sudoku constructor called" << std::endl;
  initGrid();
  generatePuzzle();
  font = load_font("../Lacquer-Regular.ttf");
  std::cout << "Sudoku constructor finished" << std::endl;
}

// Get the current state of the Sudoku board.
std::vector<std::vector<char>> Sudoku::getBoard() {
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
      cell.setOutlineColor(sf::Color::Blue);
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

// Draw the Sudoku grid and numbers.
void Sudoku::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  for (int i = 0; i < cells.size(); i++) {
    target.draw(cells[i]); // Draw the cells
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {

      // Draw thicker lines at the edge of every 3rd cell
      if (i % 3 == 0 && j % 3 == 0) {
        sf::RectangleShape line(
            sf::Vector2f(cellSize * 3, 3)); // Horizontal line
        line.setPosition(i * cellSize, j * cellSize);
        line.setFillColor(sf::Color::Blue);
        target.draw(line);

        line.setSize(sf::Vector2f(3, cellSize * 3)); // Vertical line
        line.setPosition(i * cellSize, j * cellSize);
        line.setFillColor(sf::Color::Blue);
        target.draw(line);
      }
      if (numbers[i][j] != 0) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(numbers[i][j]));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        text.setPosition(i * cellSize + 20, j * cellSize + 10);
        target.draw(text); // Draw the numbers
      }
    }
  }
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

void Sudoku::generatePuzzle() {
  // Start with a valid, filled Sudoku grid
  fillGrid();

  // Randomly remove numbers from the grid
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 8);
  for (int i = 0; i < 40; i++) {
    // TODO: adjust the number of cells to remove based on difficulty
    int row = dis(gen);
    int col = dis(gen);
    numbers[row][col] = 0;
    grid[row][col].setFillColor(sf::Color::White);
  }
}

// Handle a click event on the Sudoku grid.
void Sudoku::handleClick(int x, int y, int mouseButton) {
  int cellX = x / cellSize;
  int cellY = y / cellSize;
  if (mouseButton == sf::Mouse::Right) {
    numbers[cellX][cellY] = 0;
  } else if (mouseButton == sf::Mouse::Middle) {
    // call solver
    auto board = getBoard();
    SudokuCSP solver(board);
    solver.solve();
    board = solver.getBoard();
    std::cout << board[0][0] << std::endl;
    setBoard(board);
  } else {
    numbers[cellX][cellY] = (numbers[cellX][cellY] % 9) + 1;
  }
}

// Set the current state of the Sudoku board.
void Sudoku::setBoard(std::vector<std::vector<char>> &newBoard) {
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