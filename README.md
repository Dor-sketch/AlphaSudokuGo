# AlphaSudokuGo

AlphaSudokuGo is a C++ program designed to solve Sudoku puzzles using a `Constraint Satisfaction Problem` (CSP) approach. It features a graphical user interface (GUI) built with Qt5, which enables users to input their own puzzles and observe the solver in real-time. The primary goals of this project are to showcase the capabilities of Artificial Intelligence (AI) in tackling complex puzzles and to create cool visualizations of the search process.

<p align = "center">
  <img src="./images/test_fast/output_long.gif" alt="Sudoku puzzle" width="600" style="padding:10px; margin:10px;">
  <br>
  <i>Scroll down for more animations or <a href="#interacting-with-the-gui">jump to Interacting with the GUI</a></i>
</p>

- [Background](#background)
- [Sudoku](#sudoku)
  - [Technical Details](#technical-details)
  - [Visualization](#visualization)
  - [Area of Improvement](#area-of-improvement)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Compiling and Running the Project](#compiling-and-running-the-project)
  - [Interacting with the GUI](#interacting-with-the-gui)
- [Contributing](#contributing)
- [License](#license)

## Background

CSP is a paradigm for representing and solving problems in a declarative manner. The type of problem that CSP is designed to solve is known as a `Constraint Satisfaction Problem`, which is a problem that involves finding a solution to a set of variables subject to constraints. Well known examples of CSPs include the `N-Queens Problem`, `Map Coloring`, and `Sudoku`. Even psychometric tests utilize CSPs, in questions such as:

$$Let \ A \ and \ B \ and \ C \ be \ digits \ from \ 1 \ to \ 9.$$

$$If A \ is \ greater \ than \ B, \ What \ is \ the \ value \ of \ C \ in \ the \ following \ equation:$$

$$\frac{AAA}{BB} = C$$

or:

$$Dana \ doesn't \ want \ to \ live next \ to \ a \ person \ who \ has \ a \ dog.$$

$$Dor \ has \ a \ dog.$$

$$Eva \ doesn't \ want \ to \ live \ next \ to \ a \ person \ who \ has \ a \ cat.$$

$$Fay \ has \ a \ cat.$$

$$Order \ the \ people \ from \ left \ to \ right \ in \ a \ way \ that \ satisfies \ all \ the \ conditions.$$

or:

$$If \ A \ is \ the \ father \ of \ B, \ B \ is \ the \ father \ of \ C, \ and \ C \ is \ the \ father \ of \ D,$$$

$$who \ is \ the \ father \ of \ D?$$

Formaly, CSP is a mathematical problem defined by the tuple $(X, D, C)$. It consists of a set of variables $(X)$, their respective domains $(D)$, and a set of constraints $(C)$. The objective is to assign a value from the domain to each variable in a way that all constraints are met. CSPs are a unique category of problems that can be effectively solved using search algorithms. They find extensive applications in various fields of artificial intelligence (AI), including but not limited to scheduling, planning, and decision-making.

To illustrate, consider the Australian map-coloring problem, a simpler instance of a CSP (see [Map Coloring](https://en.wikipedia.org/wiki/Four_color_theorem)). The task here is to color each region of Australia in such a way that no two neighboring regions share the same color. This problem can be modeled as a CSP with four variables, each representing a region, and three constraints, each corresponding to a pair of neighboring regions. The aim is to discover a color assignment for the regions that fulfills all the constraints.

<p align="center">
    <img src="./images/output/0.png" alt="Australian map-coloring problem" width="400" style="padding:10px; margin:10px;">
    <img src="./images/output/7.png" alt="Australian map-coloring problem" width="400" style="padding:10px; margin:10px;">
    <br>
  <i>Australian map-coloring problem. Initial state (left) and a goal state (right)</i>
</p>

As demonstrated in the [examples.ipynb](examples.ipynb) file, the CSP in this context is composed of the following elements:

- **Variables:** These are a collection of variables, each representing a region of Australia.

  ($X = {WA, NT, Q, NSW, V, SA, T}$).

- **Domains:** These are a collection of domains, each containing the potential colors that a region can be assigned.

  ($D = {red, green, blue}$).

- **Constraints:** These are a collection of constraints, each defining the permissible color combinations for a pair of adjacent regions.

  ($C = {(WA, NT), (WA, SA), (NT, SA), (NT, Q), (SA, Q), (SA, NSW), (SA, V), (NSW, Q), (NSW, V)}$).

  It's important to note that a unary constraint is also applied to each variable, enforcing that each region must be assigned a color.

The fundamental concept of a CSP is to seek a solution that satisfies all constraints. The search space for a CSP is determined by the possible value assignments to the variables. The search algorithms employed to solve CSPs are designed to efficiently navigate this search space. There are many search algorithms that can be used to solve CSPs, including `backtracking`, `local search`, and `constraint propagation`. In this project, the `backtracking algorithm` is utilized to solve the Sudoku puzzle.

Backtracking algorithm is a general-purpose algorithm for finding all (or some) solutions to certain computational problems, notably CSPs. This algorithm incrementally constructs solution candidates and discards a candidate ("backtracks") as soon as it determines that the candidate cannot possibly be extended to a valid solution. If the algorithm identifies a variable with no remaining possible values in its domain at any point, it backtracks to the most recent variable that has a changeable value. This process repeats until a solution is discovered or it is determined that no solution is possible. The general structure of the backtracking algorithm is based on versions of the [Depth-First Search (DFS) algorithm](https://en.wikipedia.org/wiki/Depth-first_search), which explores as far as possible along each branch before backtracking.

<p align="center">
    <img src="./images/output/map_2.gif" alt="Sudoku puzzle" width="564">
    <br>
  <i>Backtracking with AC-3 Algorithm</i>
</p>

There are several strategies to enhance the backtracking algorithm, one of which is the AC-3 algorithm. This simple yet widely used algorithm enforces arc consistency in a CSP. The fundamental concept of the AC-3 algorithm is to iteratively eliminate values from the domains of the variables in a CSP until it becomes arc consistent. This is achieved by examining each constraint in the CSP and discarding values from the domains of the variables that fail to satisfy the constraint. The AC-3 algorithm is utilized to preprocess the CSP before applying the backtracking algorithm, thereby reducing the search space and enhancing the efficiency of the backtracking algorithm.

For more information on CSP you can read on the [Wikipedia page](https://en.wikipedia.org/wiki/Constraint_satisfaction_problem).

---

## Sudoku

The Sudoku puzzle serves as a classic instance of a CSP, because it involves finding a solution to a set of variables (the cells of the grid) subject to constraints.

It comprises a 9x9 grid of cells, each capable of holding a value from 1 to 9. The objective is to populate the grid so that each row, column, and 3x3 subgrid contains the numbers 1 to 9 with no repetitions. This can be modeled as a CSP with 81 variables (one for each cell), 81 domains (each containing the numbers 1 to 9), and 27 constraints (9 for the rows, 9 for the columns, and 9 for the subgrids).

My first encounter with the Sudoku puzzle was through the LeetCode problem Sudoku Solver, which is classified as a 'Hard' problem (see [Sudoku Solver](https://leetcode.com/problems/sudoku-solver/)). The challenge is to develop a program that solves a Sudoku puzzle by filling the empty cells. A Sudoku solution must adhere to all the rules previously mentioned.

To represent the Sudoku puzzle as a CSP, I utilized the following components:

- Variables: A collection of variables, each representing a cell in the Sudoku grid ($X = {x_{11}, x_{12}, \ldots, x_{99}}$).

- Domains: A collection of domains, each containing the numbers 1 to 9 ($D = {1, 2, \ldots, 9}$).

- Constraints: A collection of constraints, each defining the permissible combinations of numbers for a pair of cells

  ($C = {rows, columns, subgrids}$).

### Technical Details

To abstract the Sudoku puzzle grid, I created a hash table to store the available domains for each cell.

```c++
struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2> &pair) const {
    auto hash1 = std::hash<T1>{}(pair.first);
    auto hash2 = std::hash<T2>{}(pair.second);
    return hash1 ^ hash2;
  }
};
```

The `pair_hash` is then used inside an `unordered_map` to hold the available domains in the member variable `domains` of the class `SudokuCSP`.

```c++
class SudokuCSP {
public:
  void solve();  // Solve the Sudoku puzzle
  // ...
private:
  std::vector<std::vector<char>> board;
  std::unordered_map<std::pair<int, int>, std::unordered_set<int>, pair_hash>
      domains;
  bool isValidSudoku(std::vector<std::vector<char>> &board);
  bool is_consistent(std::pair<int, int> var, int value);
  bool backtrack();
};
```

The `backtrack` method carries out the backtracking process. This recursive function navigates the search space of the Sudoku puzzle by assigning values to the variables and verifying the satisfaction of constraints. If the algorithm encounters a variable with no remaining possible values in its domain at any point, it reverts to the most recent variable with a modifiable value. This procedure persists until a solution is identified or it is established that no solution is feasible.

```c++
bool SudokuCSP::backtrack() {
  std::pair<int, int> var = {-1, -1};
  for (int row = 0; row < 9; ++row) {
    for (int col = 0; col < 9; ++col) {
      if (board[row][col] == '.') {
        var = {row, col};
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
      if (backtrack())
        return true;
      board[var.first][var.second] = '.';
    }
  }
  return false;
}
```

### Visualization

The program includes a graphical user interface (GUI) constructed with `Qt5` and `sfml`, which allows users to input their own puzzles and watch the solver in real-time. The GUI is designed to be user-friendly and interactive, enabling users to alter the puzzle and observe the solver in action. The primary aim of the GUI is to offer an engaging and informative experience for users. The GUI employs advanced features of Qt5 and C++ to create a visually attractive and interactive environment for users. Here are some of the main features of the GUI.

```c++
// Draw the Sudoku grid and numbers.
void Sudoku::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  auto glowingColor = GlowingColor(Utility::analogousCyan).getShade();
  auto glowingBrighterColor =
      GlowingColor(Utility::analogousCyan).getBrighterShade();
  Utility::drawBackround(target, backgroundColor);
  Utility::drawLines(target, glowingColor, cellSize);
  Utility::drawNumbers(target, numbers, font, numberColor, cellSize);
  Utility::drawClosingLines(target, glowingBrighterColor, cellSize);
  saveScreenshot(target);
}

sf::Color GlowingColor::getBrighterShade() {
    float shadeFactorR = (std::sin(time) + 1) / 2; // Oscillates between 0 and 1
    float shadeFactorG = (std::sin(time + 1) + 1) / 2; // Phase shift of 1
    float shadeFactorB = (std::sin(time * 2) + 1) / 2; // Frequency of 2
    time += 0.001; // static variable to keep function state
    sf::Color shadedColor;
    shadedColor.r = std::min(static_cast<int>(baseColor.r * shadeFactorR + 50), 255);
    shadedColor.g = std::min(static_cast<int>(baseColor.g * shadeFactorG + 50), 255);
    shadedColor.b = std::min(static_cast<int>(baseColor.b * shadeFactorB + 50), 255);
    shadedColor.a = baseColor.a; // Keep the same alpha value
    return shadedColor;
}
```

The GUI supports moving-like animations, using the `GlowingColor` class to create a glowing effect on the numbers and the grid. The `draw` method is invoked every frame to update the screen and generate the animation. The GUI also supports screenshots, which can be saved using the `saveScreenshot` method, and adheres to best coding practices, such as `clean code`, `SOLID` principles, and the `DRY` principle.

The main thread of the program is tasked with managing the GUI and the user input. When the user clicks the middle mouse button, the program flow continues to update the window by passing a reference to the `window` to the `draw` method of the `SudokuCSP` class. The `draw` method is responsible for drawing the Sudoku grid and numbers, and the `GlowingColor` class is responsible for creating the glowing effect on the numbers and the grid.

Therefore, the main logic can be easily understood by reading the main loop of the program:

```c++
while (window.isOpen()) {
  handleEvents(window, sudoku);
  drawSudoku(window, sudoku);
}
```

### Area of Improvement

- The primary objective of this project is to showcase the capabilities of AI and CSP in resolving complex puzzles and to generate engaging visualizations of the search process. In doing so, the algorithm neglects to incorporate advanced techniques such as `forward checking` and `arc consistency` to enhance the efficiency of the backtracking algorithm. These techniques can be employed to reduce the search space and enhance the performance of the algorithm, by eliminates values from the domains of the variables as soon as they are assigned, thereby reducing the search space.

- There are more effective approach to backtracking in this scenario, such as selecting the order of the variables and the values to be assigned to the variables, which can be accomplished using the `Minimum Remaining Values` (MRV) and `Least Constraining Value` (LCV) heuristics. However, this would interfere with the proper functioning of the Cool Matrix visualization, as the visualization relies on the order of the variables and the values to be assigned to the variables.

- The current input format is hard-coded into the program. It could be useful to add a feature that allows users to input their own puzzles in a user-friendly manner, such as through a picture or hand-written input.

---

## Getting Started

### Prerequisites

To compile and execute AlphaSudokuGo, you will require:

- A C++ compiler that supports C++11 or later.
- A clone of this repository (it is recommended to exclude the images folder due to its large size).
- Qt5 (optional, for the GUI) and its dependencies.
- SFML (optional, for the GUI) and its dependencies.
- CMake (optional, for building the project).
- A terminal or command prompt.

If you dont have some of the required libraries installed, you can use `homebrew` to install them. For `Qt5`:

```bash
brew install qt5
```

For `SFM`:

```bash
brew install sfml
```

For `CMake`:

```bash
brew install cmake
```

If you dont have `homebrew` installed, please follow the instructions on the [Homebrew](https://brew.sh/) website.

For the interactive Jupyter notebook, you will require:

- Jupyter Notebook
- Python 3
- Matplotlib

### Compiling and Running the Project

After installing the required libraries and cloning the repository, you can compile and run the project using the following commands:

```bash
mkdir build && cd build
cmake ..
make
```

Than you can run the project using the following command,  with the optional flags `--difficulty` and `--open`:

```bash
./SudokuGame --difficulty <number of missing cells> --open <path to sudoku puzzle>
```

Without the optional `--open`, the program will initiate in a game mode with a randomly generated solvable Sudoku puzzle. You can modify the puzzle's level using the command line flag `--difficulty`, followed by the number of missing cells you want in the puzzle. The default difficulty level is set to 40 missing cells.

<p align="center">
  <img src="./images/20_missing.gif" alt="Sudoku puzzle" width="400" style="padding:10px; margin:10px;">
  <img src="./images/90_missing.gif" alt="Sudoku puzzle" width="400" style="padding:10px; margin:10px;">
  <br>
  <i>Sudoku puzzle with 1 missing cells (left) and 90 missing cells (right)</i>
</p>

### Interacting with the GUI

Click on the cell you wish to modify, each click will increment the cell value by 1. To erase a cell value, just right-click on the cell. To solve the puzzle in AI mode, press the middle mouse button (scroll wheel), and watch the solution unfold.

<p align = "center">
  <img src="./images/usage.gif" alt="Sudoku puzzle" width="400" style="padding:10px; margin:10px;">
  <img src="./images/90_solved/90_solution.gif" alt="Sudoku puzzle" width="400" style="padding:10px; margin:10px;">
  <br>
  <i>Click on the cell you wish to modify (left) and watch the solution unfold (right)</i>
</p>

You can also change the theme of the game, including fonts and styles. In order to change font you will need to download a `.ttf` file and place it in the program directory. Than you can change the font by modifying the `font` variable in the `Sudoku.cpp` file.To change the style of the game, you can modify the `backgroundColor`, `numberColor`, and `cellSize` variables in the `Sudoku.cpp` file. Here are some examples of the different themes:

<p align = "center">
  <i>Light mode</i>
  <br>
  <img src="./images/animation3.gif"            alt="Sudoku puzzle Light mode" width="400" style="padding:10px; margin:10px;">
  <img src="./images/test2/animation_green.gif" alt="Sudoku puzzle Light mode" width="400" style="padding:10px; margin:10px;">
  <br>
  <i>Glowing mode</i>
  <br>
  <img src="./images/glowing1/glowing.gif"  alt="Sudoku puzzle Glowing mode" width="400" style="padding:10px; margin:10px;">
  <img src="./images/glowing2/glowing2.gif" alt="Sudoku puzzle Glowing mode" width="400" style="padding:10px; margin:10px;">
  <img src="./images/glowing2/glowing3.gif" alt="Sudoku puzzle Glowing mode" width="400" style="padding:10px; margin:10px;">
  <img src="./images/glowing2/glowing5.gif" alt="Sudoku puzzle Glowing mode" width="400" style="padding:10px; margin:10px;">
  <br>
  <i>Dark mode</i>
  <br>
  <img src = "/images/test9/output9.gif" alt="Sudoku puzzle Dark mod" width="400" style="padding:10px; margin:10px;">
  <img src="./images/test10/output8.gif" alt="Sudoku puzzle Dark mod" width="400" style="padding:10px; margin:10px;">
</p>

## Contributing

Any contributions you make are greatly appreciated.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
