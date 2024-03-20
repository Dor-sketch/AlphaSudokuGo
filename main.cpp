#include "Sudoku.h"
#include "SudokuCSP.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void solveSudoku(Sudoku &sudoku, sf::RenderWindow &window) {
  SudokuCSP csp(window, sudoku);
  csp.solve();
}

void handleEvents(sf::RenderWindow &window, Sudoku &sudoku) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
      int mouseButton = event.mouseButton.button;
      if (mouseButton == sf::Mouse::Middle) {
        sudoku.setDarkTheme();
        solveSudoku(sudoku, window);
      } else {
        int x = event.mouseButton.x;
        int y = event.mouseButton.y;
        sudoku.handleClick(x, y, mouseButton);
      }
    }
  }
}
void drawSudoku(sf::RenderWindow &window, Sudoku &sudoku) {
  window.clear();
  window.draw(sudoku);
  window.display();
}

int main(int argc, char *argv[]) {
  sf::Text text;
  sf::RenderWindow window(
      sf::VideoMode(64 * 9, 64 * 9), "AlphaSudokuGo",
      sf::Style::Close); // dont allow changing the window size

  std::string difficulty;
  std::string filename;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--difficulty" && i + 1 < argc) {
      difficulty = argv[++i];
    } else if (arg == "--open" && i + 1 < argc) {
      filename = argv[++i];
    }
  }

  Sudoku sudoku(difficulty, filename);

  while (window.isOpen()) {
    handleEvents(window, sudoku);
    drawSudoku(window, sudoku);
  }

  return 0;
}