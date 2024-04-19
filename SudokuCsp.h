#pragma once

#include "Sudoku.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Particals.h"
// Define a hash function for using pairs as keys in unordered_map
struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const {
    auto hash1 = std::hash<T1>{}(pair.first);
    auto hash2 = std::hash<T2>{}(pair.second);
    return hash1 + hash2 * 8; // Copilot: "8 is a random prime number"
    // Me: "8 is beacuse we have 9*9=81 cells in the sudoku board, 0-indexed"
  }
};

class SudokuCSP {
public:
  SudokuCSP(sf::RenderWindow &window, Sudoku &sudoku, Effects &effects);
  const std::vector<std::vector<char>> getBoard(); // Get the Sudoku board
  bool solve();                                    // Solve the Sudoku puzzle

private:
Effects &effects;
  void AC3();
  bool isArcConsistent(std::pair<int, int> var);
  sf::RenderWindow &window;
  Sudoku &sudoku;
  bool isValidSudoku(std::vector<std::vector<char>> &board);
  std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pair_hash>
      domains;
  std::vector<std::vector<char>> board;
  bool is_consistent(std::pair<int, int> var, int value);
  bool backtrack();
};
