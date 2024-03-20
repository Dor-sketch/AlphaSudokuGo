#include "Sudoku.h"
#include "SudokuCsp.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void handleEvents(sf::RenderWindow &window, Sudoku &sudoku) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
      int mouseButton = event.mouseButton.button;
      int x = event.mouseButton.x;
      int y = event.mouseButton.y;
      sudoku.handleClick(x, y, mouseButton);
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