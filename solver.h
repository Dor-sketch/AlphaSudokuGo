# pragma once

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "sudoku.h"

// Define a hash function for using pairs as keys in unordered_map
struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const {
    auto hash1 = std::hash<T1>{}(pair.first);
    auto hash2 = std::hash<T2>{}(pair.second);
    return hash1 ^ hash2;
  }
};

class SudokuCSP {
public:
  SudokuCSP(std::vector<std::vector<char>> &board, Sudoku & sudoku);
  const std::vector<std::vector<char>> getBoard();
  void solve();
  Sudoku sudoku;

private:
  bool isValidSudoku(std::vector<std::vector<char>> &board);
  std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pair_hash>
      domains;
  std::vector<std::vector<char>> &board;
  bool is_consistent(std::pair<int, int> var, int value);
  bool backtrack();
};
