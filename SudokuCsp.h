# pragma once

#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
  SudokuCSP(std::vector<std::vector<char>> &board);
  const std::vector<std::vector<char>> getBoard();  // Get the Sudoku board
  void solve();  // Solve the Sudoku puzzle

private:
  bool isValidSudoku(std::vector<std::vector<char>> &board);
  std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pair_hash>
      domains;
  std::vector<std::vector<char>> &board;
  bool is_consistent(std::pair<int, int> var, int value);
  bool backtrack();
};
