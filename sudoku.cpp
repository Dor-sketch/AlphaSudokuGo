#include "Sudoku.h"
#include "SudokuCsp.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

sf::Color complementaryPurple(128, 0, 128); // A shade of purple
sf::Color analogousCyan(0, 255, 255);       // A shade of cyan
sf::Color triadicYellow(255, 255, 0);       // A shade of yellow
const int cellSize = 64;

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
  initGrid();
  generatePuzzle();
  font = load_font("../Lacquer-Regular.ttf");
}

Sudoku::Sudoku(const std::vector<std::vector<char>> &board) {
  setBoard(board);
  font = load_font("../VT323-Regular.ttf");
  sf::RenderTexture renderTexture;

  // Create a RenderTexture to draw on
  if (!renderTexture.create(9 * cellSize, 9 * cellSize)) {
    std::cerr << "Failed to create RenderTexture" << std::endl;
    return;
  }

  renderTexture.clear(sf::Color::Transparent);
  drawText(renderTexture);
  drawGridLines(renderTexture);
  drawBoldLines(renderTexture);
  drawGridLines(renderTexture);
  drawClosingLines(renderTexture);
  saveScreenshot(renderTexture);
}

void Sudoku::drawText(sf::RenderTexture &renderTexture) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (numbers[i][j] == 0) {
        continue;
      }
      sf::Text text;
      text.setFont(font);
      text.setString(std::to_string(numbers[i][j]));
      text.setCharacterSize(24);
      text.setFillColor(sf::Color::Green);
      text.setPosition(i * cellSize + 25, j * cellSize + 20);
      renderTexture.draw(text);
    }
  }
}

void Sudoku::drawGridLines(sf::RenderTexture &renderTexture) {
  for (int i = 1; i <= 9; i++) {
    sf::RectangleShape line(sf::Vector2f(9 * cellSize, 1));
    line.setPosition(i * cellSize, 0);
    line.setFillColor(triadicYellow);
    renderTexture.draw(line);

    line.setSize(sf::Vector2f(1, 9 * cellSize));
    line.setPosition(0, i * cellSize);
    line.setFillColor(triadicYellow);
    renderTexture.draw(line);

  }
}

void Sudoku::drawBoldLines(sf::RenderTexture &renderTexture) {
  for (int i = 0; i <= 9; i++) {
    for (int j = 0; j <= 9; j++) {
      if (i % 3 == 0 && j % 3 == 0) {
        sf::RectangleShape line(
            sf::Vector2f(cellSize * 3 - 1, 3 - 1)); // Horizontal line
        line.setPosition(i * cellSize, j * cellSize);
        line.setFillColor(analogousCyan);
        renderTexture.draw(line);

        line.setSize(sf::Vector2f(3, cellSize * 3)); // Vertical line
        if (i == 9)
          line.setSize(sf::Vector2f(3, cellSize * 3 - 1));
        else if (j == 9)
          line.setSize(sf::Vector2f(cellSize * 3 - 1, 3));
        line.setFillColor(triadicYellow);
        renderTexture.draw(line);
      }
    }
  }
}

void Sudoku::saveScreenshot(sf::RenderTexture &renderTexture) {
  sf::Image board_image = renderTexture.getTexture().copyToImage();
  std::time_t result = std::time(nullptr);
  std::string filename = "board" + std::to_string(result) + ".png";
  if (!board_image.saveToFile(filename)) {
    std::cerr << "Failed to save screenshot" << std::endl;
  }
}

void Sudoku::drawClosingLines(sf::RenderTexture &renderTexture) {
  sf::RectangleShape line(sf::Vector2f(9 * cellSize, 3));
  line.setPosition(0, 9 * cellSize - 1);
  line.setFillColor(analogousCyan);
  renderTexture.draw(line);
  line.setSize(sf::Vector2f(3, 9 * cellSize));
  line.setPosition(9 * cellSize - 1, 0);
  line.setFillColor(analogousCyan);
  renderTexture.draw(line);
  renderTexture.display();
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
        text.setPosition(i * cellSize + 25, j * cellSize + 20);
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
    auto solvedBoard = solver.getBoard();
    setBoard(solvedBoard);
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