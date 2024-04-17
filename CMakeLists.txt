cmake_minimum_required(VERSION 3.10)
project(SudokuGame)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(Qt5_DIR "/usr/local/Cellar/qt/5.15.2/lib/cmake/Qt5")
find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SFML_DIR "/usr/local/Cellar/sfml/2.6.1/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics audio REQUIRED)

set(SOURCES
    Parser.cpp
    main.cpp
    Sudoku.cpp
    SudokuCSP.cpp
    Particals.cpp
    GlowingColor.cpp
    Utility.cpp
)
set(HEADERS
    Parser.h
    GlowingColor.h
    Sudoku.h
    SudokuCSP.h
    Particals.h
    Utility.h
)

link_directories(/usr/local/Cellar/opencv/4.9.0_7.reinstall/lib /usr/local/Cellar/tesseract/5.3.4_1/lib /usr/local/Cellar/leptonica/1.84.1/lib)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE /usr/local/Cellar/opencv/4.9.0_7.reinstall/include/opencv4 /usr/local/Cellar/tesseract/5.3.4_1/include /usr/local/Cellar/leptonica/1.84.1/include)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets sfml-graphics sfml-audio opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs tesseract leptonica )