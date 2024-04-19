#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

extern std::vector<std::vector<char>>
boardsFactory(const std::string &filename);

class SudokuImageParser {
public:
  SudokuImageParser(const cv::Mat &inputImage);

  std::string parse();
  cv::Mat image;
};


class BoardsFactory {
public:
  BoardsFactory();
  BoardsFactory(const std::string &inputImage_path);
  std::vector<std::vector<char>> getBoard();
  void rotateBoard();
  void mirrorBoard();
  void rotate_CounterClockwise();
  void rotate_Clockwise();
  std::string getScreenShot();
  int getScreen_x();
  int getScreen_y();
  int getCell_size();
  std::vector<std::vector<char>> getEmptyBoard();
  private:
  void setBoard();
  std::string exec(const char *cmd);
  cv::Mat parseImg(const std::string &inputImage_path);
  void extractBoard( cv::Mat &inputImage);
  void parseCells(const cv::Mat &inputImage, int board[9][9]);
  std::vector<std::vector<char>> board;
  int cell_size;
  int screen_x;
  int screen_y;
  std::string inputImage_path = "sudoku.jpg";
};
