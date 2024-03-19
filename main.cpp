#include "solver.h"
#include "sudoku.h"
#include <iostream>
#include <vector>
#include <string>
#include <thread>

void handleEvents(sf::RenderWindow &window, Sudoku &sudoku) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
      int mouseButton = event.mouseButton.button;
      if (mouseButton == sf::Mouse::Middle) {
        // call solver new thread
        auto board = sudoku.getBoard();
        SudokuCSP solver(board, sudoku);
        std::thread t1(&SudokuCSP::solve, &solver);
        t1.detach();
      }
      else {
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

int main() {
  sf::Text text;
  Sudoku sudoku;
  sf::RenderWindow window(
      sf::VideoMode(64 * 9, 64 * 9), "AlphaSudokuGo",
      sf::Style::Close); // dont allow changing the window size
  while (window.isOpen()) {
    handleEvents(window, sudoku);
    drawSudoku(window, sudoku);
  }

  return 0;
}