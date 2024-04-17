#include "Parser.h"
#include "Particals.h"
#include "Sudoku.h"
#include "SudokuCSP.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

extern std::vector<std::vector<char>>
boardsFactory(const std::string &filename);
static int effect_type = 4;

void solveSudoku(Sudoku &sudoku, sf::RenderWindow &window) {
  SudokuCSP csp(window, sudoku);
  csp.solve();
  // kill the thread
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sudoku.setBoard(csp.getBoard());
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
    } else if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::S) {
        sudoku.saveScreenshot(window);
      }
      if (event.key.code == sf::Keyboard::P) {
        // increase particle count
        sudoku.particleCount += 100;
      } else if (event.key.code == sf::Keyboard::R) {
        sudoku.setBoard(boardsFactory("empty"));
        sudoku.setLightTheme();
      } else if (event.key.code == sf::Keyboard::D) {
        sudoku.setDarkTheme();
      } else if (event.key.code == sf::Keyboard::Escape) {
        window.close();
      } else if (event.key.code == sf::Keyboard::Space) {
        solveSudoku(sudoku, window);
      } else if (event.key.code == sf::Keyboard::Up) {
        sudoku.setBoard(boardsFactory("sudoku.jpg"));
      } else if (event.key.code == sf::Keyboard::N) {
        effect_type = (effect_type + 1) % 4;
      } else {
        // print instructions
        std::cout << "Press R to reset the board\n"
                  << "Press D to switch to dark theme\n"
                  << "Press S to save a screenshot\n"
                  << "Press Space to solve the Sudoku\n"
                  << "Press Up to load a new Sudoku\n"
                  << "Press N to change the effect\n";
      }
    } else if (event.type == sf::Event::TextEntered) {
      if (event.text.unicode == 27) {
        window.close();
      }
    } else if (event.type == sf::Event::Resized) {
      window.setSize({event.size.width, event.size.height});
    }
    // elif (event.type == sf::Event::GainedFocus) {
    //   sudoku.setDarkTheme();
    // }
  }
}
void drawSudoku(sf::RenderWindow &window, Sudoku &sudoku, Effects &effects) {
  window.clear();
  window.draw(sudoku);
  effects.apply(effect_type, sudoku.getBoard());
  window.display();
}

int main(int argc, char *argv[]) {
  sf::Text text;
  sf::RenderWindow window(
      sf::VideoMode(64 * 9, 64 * 9), "AlphaSudokuGo",
      sf::Style::Close); // dont allow changing the window size
  // clock
  sf::Clock clock;

  std::string difficulty;
  std::string filename;
  bool screenShot = false;
  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--difficulty" && i + 1 < argc) {
      difficulty = argv[++i];
    } else if (arg == "--open" && i + 1 < argc) {
      filename = argv[++i];
    } else if (arg == "--screen") {
      filename = "screen.jpg";
      screenShot = true;
    } else if (arg == "--help") {
      std::cout
          << "Usage: ./AlphaSudokuGo [OPTIONS]\n"
          << "Options:\n"
          << "  --difficulty <easy|medium|hard>  Set the difficulty level\n"
          << "  --open <filename>                 Open a Sudoku puzzle from a "
             "file (.jpg or .txt)\n"
          << "  --screen                          Open a Sudoku puzzle from "
             "the "
             "screen\n";
      return 0;
    } else {
      std::cerr << "Unknown option: " << arg << std::endl;
      return 1;
    }
  }

  Sudoku sudoku(difficulty, filename);
  Effects effects(window, clock, 10, sudoku.getBoard());

  while (window.isOpen()) {
    handleEvents(window, sudoku);
    drawSudoku(window, sudoku, effects);
  }

  return 0;
}