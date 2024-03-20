

cmake_minimum_required(VERSION 3.10)
project(SudokuGame)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(Qt5_DIR "/usr/local/Cellar/qt/5.15.2/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Widgets REQUIRED)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Define the SFML directory
set(SFML_DIR "/usr/local/Cellar/sfml/2.6.1/lib/cmake/SFML")

# Find the SFML package
find_package(SFML 2.6 COMPONENTS graphics audio REQUIRED)

# Define your sources and headers. Add all relevant files.
set(SOURCES
    main.cpp
    Sudoku.cpp
    SudokuCSP.cpp
)
set(HEADERS
    Sudoku.h
    SudokuCSP.h
)

# Define the executable and link it with the Qt5Widgets library.
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link the SFML libraries to your target
target_link_libraries(${PROJECT_NAME} Qt5::Widgets sfml-graphics sfml-audio)