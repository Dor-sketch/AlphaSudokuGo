#include "solver.h"

bool SudokuCSP::is_consistent(std::pair<int, int> var, int value) {
  int row = var.first, col = var.second;
  // Check row and column
  for (int k = 0; k < 9; ++k) {
    if (board[row][k] == value + '0' || board[k][col] == value + '0')
      return false;
  }
  // Check 3x3 sub-box
  int startRow = row - row % 3, startCol = col - col % 3;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (board[startRow + i][startCol + j] == value + '0')
        return false;
    }
  }
  return true;
}

bool SudokuCSP::backtrack() {
  std::pair<int, int> var = {-1, -1};
  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      sudoku.updateBoard(getBoard());
      if (board[row][col] == '.') {
        var = {row, col};
        std::cout << "Unassigned cell: " << row << " " << col << std::endl;
        goto found_unassigned;
      }
    }
  }
found_unassigned:
  if (var.first == -1)
    return true; // Puzzle solved

  for (int val : domains[var]) {
    if (is_consistent(var, val)) {
      board[var.first][var.second] = val + '0';
      std::cout<<"saving screenshot"<<std::endl;
      Sudoku(getBoard());
      if (backtrack())
        return true;
      board[var.first][var.second] = '.';
    }
  }

  // print the board
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      std::cout << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
  return false;
}

SudokuCSP::SudokuCSP(std::vector<std::vector<char>> &board,Sudoku & sudoku) : board(board),sudoku(sudoku) {
  // check if the board is valid
  if (!isValidSudoku(board)) {
    std::cout << "Invalid Sudoku board" << std::endl;
    return;
  }
  // Initialize the domains for each unassigned cell
  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      if (board[row][col] == '.') {
        domains[{row, col}] =
            std::unordered_set<int>{1, 2, 3, 4, 5, 6, 7, 8, 9};
      }
    }
  }
}

void SudokuCSP::solve() {
  std::cout << "Solving..." << std::endl;
  backtrack();
}

const std::vector<std::vector<char>> SudokuCSP::getBoard() { return board; }

// Check if the current state of the Sudoku board is valid.
bool SudokuCSP::isValidSudoku(std::vector<std::vector<char>> &board) {
  int seenRows[9] = {0}, seenCols[9] = {0}, seenSquares[9] = {0};

  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      char c = board[i][j];
      if (c != '.') {
        int val = c - '0';  // Convert char to int
        int bit = 1 << val; // Bitmask for the digit

        // Check row
        if (seenRows[i] & bit)
          return false;
        seenRows[i] |= bit;

        // Check column
        if (seenCols[j] & bit)
          return false;
        seenCols[j] |= bit;

        // Check square
        int squareIndex = (i / 3) * 3 + j / 3;
        if (seenSquares[squareIndex] & bit)
          return false;
        seenSquares[squareIndex] |= bit;
      }
    }
  }
  return true;
}
