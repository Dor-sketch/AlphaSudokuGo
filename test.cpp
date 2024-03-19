// pass to the soduku to solver actions one by one

# include <iostream>
# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <SFML/Graphics.hpp>
# include <SFML/Window.hpp>
# include <SFML/System.hpp>


#include "solver.h"
#include "sudoku.h"

class Tester {
public:
    Tester() {
        std::cout << "Tester constructor called" << std::endl;
        Sudoku sudoku;
        std::vector<std::vector<char>> board = sudoku.getBoard();
        SudokuCSP solver(board);
        solver.solve();
        solution = board;
        std::cout << "Tester constructor finished" << std::endl;
    };

    std::vector<std::vector<char>> solution;
    int moving_x;
    int moving_y;
    sf::Event pass_mouse_clicks_to_gui() {
        // get the mouse click needed for the GUI according to the solution
        int num_of_clicks = solution[moving_x][moving_y] - '0';  // Use moving_y for the second dimension
        int cur_clicks = moving_x * 9 + moving_y;

        sf::Event event = sf::Event();
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = (cur_clicks % 9) * 64 + 32;
        event.mouseButton.y = (cur_clicks / 9) * 64 + 32;
        return event;
    };
};