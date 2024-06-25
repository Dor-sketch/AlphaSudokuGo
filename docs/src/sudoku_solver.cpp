#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <emscripten/bind.h>
using namespace std;

bool found = false;

// Define a hash function for using pairs as keys in unordered_map
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 * 11 + hash2;
    }
};

class SudokuCSP {
private:
    vector<vector<char>> board;
    unordered_map<pair<int_fast8_t, int_fast8_t>, unordered_set<int_fast8_t>, pair_hash> domains;

    void AC3() {
        std::queue<std::pair<int_fast8_t, int_fast8_t>> q;
        for (int_fast8_t i = 0; i < 9; ++i) {
            for (int_fast8_t j = 0; j < 9; ++j) {
                if (board[i][j] != '.') {
                    q.push({i, j});
                }
            }
        }
        while (!q.empty()) {
            auto cell = q.front();
            q.pop();
            int_fast8_t row = cell.first, col = cell.second;
            int_fast8_t value = board[row][col] - '0';
            int_fast8_t startRow = row - row % 3, startCol = col - col % 3;
            for (int_fast8_t i = 0; i < 9; ++i) {
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
            for (int_fast8_t i = 0; i < 3; ++i) {
                for (int_fast8_t j = 0; j < 3; ++j) {
                    int_fast8_t r = startRow + i, c = startCol + j;
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

    bool is_consistent(pair<int_fast8_t, int_fast8_t> var, int_fast8_t value) {
        int_fast8_t row = var.first, col = var.second;
        for (int_fast8_t k = 0; k < 9; ++k) {
            if (board[row][k] == value + '0' || board[k][col] == value + '0')
                return false;
        }
        int_fast8_t startRow = row - row % 3, startCol = col - col % 3;
        for (int_fast8_t i = 0; i < 3; ++i) {
            for (int_fast8_t j = 0; j < 3; ++j) {
                if (board[startRow + i][startCol + j] == value + '0')
                    return false;
            }
        }
        return true;
    }

    bool backtrack() {
        if (found)
            return true;
        pair<int_fast8_t, int_fast8_t> var = {-1, -1};
        for (int_fast8_t row = 0; row < 9; ++row) {
            for (int_fast8_t col = 0; col < 9; ++col) {
                if (board[row][col] == '.') {
                    var = {row, col};
                    goto found_unassigned;
                }
            }
        }
    found_unassigned:
        if (var.first == -1)
            return true;
        for (int_fast8_t val : domains[var]) {
            if (is_consistent(var, val)) {
                board[var.first][var.second] = val + '0';
                try {
                    if (!found) {
                        if (backtrack()) {
                            found = true;
                            return true;
                        }
                    }
                } catch (const std::runtime_error& e) {
                    return true;
                }
                board[var.first][var.second] = '.';
            }
        }
        return false;
    }

public:
    SudokuCSP(string boardStr) {
        board.resize(9, vector<char>(9, '.'));
        for (int_fast8_t i = 0; i < 9; ++i) {
            for (int_fast8_t j = 0; j < 9; ++j) {
                board[i][j] = boardStr[i * 9 + j];
                if (board[i][j] == '.') {
                    domains[{i, j}] = unordered_set<int_fast8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9};
                }
            }
        }
        AC3();
    }

    void solve() {
        found = false;
        backtrack();
    }

    string getBoard() {
        string result;
        for (const auto& row : board) {
            for (char cell : row) {
                result += cell;
            }
        }
        return result;
    }
};

EMSCRIPTEN_BINDINGS(sudoku_module) {
    emscripten::class_<SudokuCSP>("SudokuCSP")
        .constructor<string>()
        .function("solve", &SudokuCSP::solve)
        .function("getBoard", &SudokuCSP::getBoard);
}