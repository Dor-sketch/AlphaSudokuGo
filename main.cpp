#include "Parser.h"
#include "Particals.h"
#include "Sudoku.h"
#include "SudokuCSP.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

static int NumberOfEffects = 11;
static int effect_type = -1;
static int cellSize = 50;

void solveSudoku(Sudoku &sudoku, sf::RenderWindow &window, Effects &effects) {
  try {
    SudokuCSP csp(window, sudoku, effects);
    sudoku.setDarkTheme();
    effect_type = 3;
    csp.solve();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sudoku.setBoard(csp.getBoard());
    effect_type = 1;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return;
  }
}

void handleEvents(sf::RenderWindow &window, Sudoku &sudoku, BoardsFactory& boardsFactory, Effects &effects) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
      int mouseButton = event.mouseButton.button;
      if (mouseButton == sf::Mouse::Middle) {
        solveSudoku(sudoku, window, effects);
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
        effect_type = -1;
        sudoku.setBoard(boardsFactory.getEmptyBoard());
        sudoku.setLightTheme();
      } else if (event.key.code == sf::Keyboard::D) {
        sudoku.setDarkTheme();
      } else if (event.key.code == sf::Keyboard::Escape) {
        window.close();
      } else if (event.key.code == sf::Keyboard::Space) {
        solveSudoku(sudoku, window, effects);
      }
      // else if (event.key.code == sf::Keyboard::Up) {
      //   boardsFactory = BoardsFactory();
      //   sudoku.setBoard(boardsFactory.getBoard());
      // }
      else if (event.key.code == sf::Keyboard::N) {
        effect_type = (effect_type + 1) % NumberOfEffects;
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
    } else if (arg == "--open" && i + 1 >= argc) {
      std::cerr << "Missing filename argument for --open" << std::endl;
      return 1;
    } else if (arg == "--size" && i + 1 < argc) {
      cellSize = std::stoi(argv[++i]);
    } else if (arg == "--size" && i + 1 >= argc) {
      std::cerr << "Missing size argument for --size" << std::endl;
      return 1;
    }
    else if (arg == "--screen") {
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

  BoardsFactory boardsFactory = BoardsFactory();
  cellSize = boardsFactory.getCell_size();
  std::cout << cellSize << std::endl;

  auto board = boardsFactory.getBoard();
  auto screen_x = boardsFactory.getScreen_x();
  auto screen_y = boardsFactory.getScreen_y();
sf::RenderWindow window(sf::VideoMode(cellSize * 9, cellSize * 9), "Alpha Sudoku Go");
  // Move the window to the desired position
  Sudoku sudoku(board, cellSize);
  Effects effects(window, clock, 10, sudoku.getBoard());
  // move the window to the desired position
  auto last_clicked = sf::Mouse::getPosition();
  // calculate the position of the window based on the last clicked position
  screen_x = last_clicked.x;
  screen_y = last_clicked.y - cellSize * 9;


  window.setPosition({screen_x, screen_y});

  while (window.isOpen()) {
    handleEvents(window, sudoku, boardsFactory, effects);
    drawSudoku(window, sudoku, effects);
  }

  return 0;
}