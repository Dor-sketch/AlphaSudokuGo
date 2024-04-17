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