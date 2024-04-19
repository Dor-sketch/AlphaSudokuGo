#include "SudokuCSP.h"
#include "Sudoku.h"
#include "Particals.h"

const bool storeScreenshot = false;
  static int effect_type = 0;
bool found = false;
SudokuCSP::SudokuCSP(sf::RenderWindow &window, Sudoku &sudoku, Effects &effects)
    : sudoku(sudoku), window(window) , effects(effects) {
  // check if the board is valid
  board = sudoku.getBoard();
  if (!isValidSudoku(board)) {
    std::cout << "Invalid Sudoku board" << std::endl;
    throw std::invalid_argument("Invalid Sudoku board");
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
  // Apply AC3 to reduce the domains
  AC3();
}

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

void SudokuCSP::AC3() {
  std::queue<std::pair<int, int>> q;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (board[i][j] != '.') {
        q.push({i, j});
      }
    }
  }
  while (!q.empty()) {
    auto cell = q.front();
    q.pop();
    int row = cell.first, col = cell.second;
    int value = board[row][col] - '0';
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 9; ++i) {
      if (i != row && domains[{i, col}].erase(value)) {
        if (domains[{i, col}].empty())
          return;
        if (domains[{i, col}].size() == 1)
          q.push({i, col});
      }
      if (i != col && domains[{row, i}].erase(value)) {
        if (domains[{row, i}].empty())
          return;
        if (domains[{row, i}].size() == 1)
          q.push({row, i});
      }
    }
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        int r = startRow + i, c = startCol + j;
        if (r != row && c != col && domains[{r, c}].erase(value)) {
          if (domains[{r, c}].empty())
            return;
          if (domains[{r, c}].size() == 1)
            q.push({r, c});
        }
      }
    }
  }
}

void updateBoard(std::vector<std::vector<char>> &board, std::pair<int, int> var,
                 int value) {
  board[var.first][var.second] = value + '0';
}
void updateWindow(sf::RenderWindow &window, Sudoku &sudoku,
                  std::vector<std::vector<char>> &board, Effects &effects) {
  sudoku.setBoard(board);
  window.clear();
  window.draw(sudoku);
  effects.apply(effect_type, board);
  window.display();
  // option to store a screenshot of the window

  // Change effect_type when 'n' is pressed
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
    effect_type = (effect_type + 1) % 11; // Cycle through available effects
  }
}

bool SudokuCSP::isArcConsistent(std::pair<int, int> var) {
  std::queue<std::pair<int, int>> q;
  for (int i = 0; i < 9; ++i) {
    if (i != var.first)
      q.push({i, var.second});
    if (i != var.second)
      q.push({var.first, i});
  }
  int startRow = var.first - var.first % 3,
      startCol = var.second - var.second % 3;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      int row = startRow + i, col = startCol + j;
      if (row != var.first && col != var.second)
        q.push({row, col});
    }
  }
  while (!q.empty()) {
    auto cell = q.front();
    q.pop();
    int row = cell.first, col = cell.second;
    if (board[row][col] == '.') {
      std::unordered_set<int> &domain = domains[{row, col}];
      for (auto it = domain.begin(); it != domain.end();) {
        int value = *it;
        if (!is_consistent({row, col}, value)) {
          it = domain.erase(it);
        } else {
          ++it;
        }
      }
      if (domain.empty())
        return false;
    }
  }
  return true;
}

bool SudokuCSP::backtrack() {
  std::pair<int, int> var = {-1, -1};
  // sort the variables based on the number of constraints - most constrained
  // variable select the variable with the least number of values in its domain
  auto oldDomains = domains;
  AC3();
  for (auto &[cell, domain] : domains) {
    if (board[cell.first][cell.second] == '.') {
      if (var.first == -1 || domain.size() < domains[var].size()) {
        var = cell;
      }
    }
  }
  bool found_unassigned = false;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      if (board[i][j] == '.') {
        updateWindow(window, sudoku, board, effects);
        goto found_unassigned;
      }
    }
  }
found_unassigned:
  if (var.first == -1)
    return true; // Puzzle solved
  // sort the values in the domain of the variable
  // based on the number of constraints - least constraining value
  std::vector<std::pair<int, int>> sortedValues;
  for (int val : domains[var]) {
    int count = 0;
    for (int i = 0; i < 9; ++i) {
      if (is_consistent({var.first, var.second}, val)) {
        count++;
      }
    }
    sortedValues.push_back({count, val});
  }
  std::sort(sortedValues.begin(), sortedValues.end());

  for (auto [_, val] : sortedValues) {
    if (is_consistent(var, val)) {
      board[var.first][var.second] = val + '0';
      updateWindow(window, sudoku, board, effects);
      if (backtrack()) {
        updateWindow(window, sudoku, board, effects);
        return true;
      }

      board[var.first][var.second] = '.';
    }
  }
  domains = oldDomains;
  return false;
}

bool SudokuCSP::solve() { return backtrack(); }

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
