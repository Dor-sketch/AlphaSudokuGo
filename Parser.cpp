#include "Parser.h"
#include <iostream>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

cv::Mat warpImage(const cv::Mat &image, const std::vector<cv::Point> &contour) {
  auto rect = cv::minAreaRect(contour);
  cv::Point2f points[4];
  rect.points(points);

  // Calculate the width and height of the Sudoku grid
  float width = cv::norm(points[0] - points[1]);
  float height = cv::norm(points[1] - points[2]);

  // Calculate the destination points
  cv::Point2f dstPoints[4] = {cv::Point(0, 0), cv::Point(width - 1, 0),
                              cv::Point(width - 1, height - 1),
                              cv::Point(0, height - 1)};

  // Create the perspective transformation matrix
  cv::Mat transform = cv::getPerspectiveTransform(points, dstPoints);

  // Apply the perspective transformation
  cv::Mat warped;
  cv::warpPerspective(image, warped, transform, cv::Size(width, height));

  return warped;
}

std::string SudokuImageParser::parse() {
  // Initialize Tesseract
  tesseract::TessBaseAPI tess;
  if (tess.Init(NULL, "eng", tesseract::OEM_DEFAULT) != 0) {
    throw std::runtime_error("Failed to initialize Tesseract");
  }
  tess.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  int depth = CV_MAT_DEPTH(image.type()) * 8;
  switch (image.depth()) {
  case CV_8U:
  case CV_8S:
    depth = 8;
    break;
  case CV_16U:
  case CV_16S:
    depth = 16;
    break;
  case CV_32S:
  case CV_32F:
    depth = 32;
    break;
  case CV_64F:
    depth = 64;
    break;
  default:
    throw std::runtime_error("Unsupported image depth");
  }
  // Convert cv::Mat to Pix
  // Convert cv::Mat// Convert cv::Mat to Pix
  // Convert cv::Mat to Pix
  // Convert cv::Mat to Pix
  // print for debug
  for (int i = 0; i < image.rows; i++) {
    for (int j = 0; j < image.cols; j++) {
      std::cout << image.at<uchar>(i, j) << " ";
    }
    std::cout << std::endl;
  }

  // Create a new l_uint32 array and copy the data from the cv::Mat
  l_uint32 *data = new l_uint32[image.total()];

  // Create the Pix object
  Pix *pix = pixCreateHeader(image.rows, image.cols, depth);
  pixSetData(pix, data);
  tess.SetImage(pix);

  // Get the text from the image
  char *out = tess.GetUTF8Text();
  std::string text(out);
  return text;
}

SudokuImageParser::SudokuImageParser(const cv::Mat &inputImage)
    : image(inputImage) {}

Pix *mat8ToPix(cv::Mat *mat8) {
  mat8->convertTo(*mat8, CV_8UC1);
  Pix *pix = pixCreate(mat8->size().width, mat8->size().height, 8);
  for (int i = 0; i < mat8->rows; i++) {
    for (int j = 0; j < mat8->cols; j++) {
      pixSetPixel(pix, j, i, (l_uint32)mat8->at<uchar>(i, j));
    }
  }
  return pix;
}

std::vector<std::vector<char>> itochar(int board[9][9]) {
  std::vector<std::vector<char>> board_char;
  for (int i = 0; i < 9; i++) {
    std::vector<char> row;
    for (int j = 0; j < 9; j++) {
      if (board[i][j] == 0) {
        row.push_back('.');
      } else {
        row.push_back(board[i][j] + '0');
      }
    }
    board_char.push_back(row);
  }
  return board_char;
}

void BoardsFactory::extractBoard( cv::Mat &inputImage) {
  // Apply Gaussian blur
  cv::GaussianBlur(inputImage, inputImage, cv::Size(5, 5), 0);

  // Use Canny edge detection
  cv::Mat edges;
  cv::Canny(inputImage, edges, 50, 150);

  // Find contours
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(edges, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

  // Find the largest 4-sided contour
  std::vector<cv::Point> sudokuContour;
  double maxArea = 0;
  for (const auto &contour : contours) {
    double area = cv::contourArea(contour);
    if (area > maxArea) {
      std::vector<cv::Point> approx;
      cv::approxPolyDP(contour, approx, cv::arcLength(contour, true) * 0.02,
                       true);
      if (approx.size() == 4) {
        sudokuContour = approx;
        maxArea = area;
      }
    }
  }

  // If a Sudoku board was found, extract it from the image
  if (!sudokuContour.empty()) {
    cv::Rect sudokuRect = cv::boundingRect(sudokuContour);
    inputImage = inputImage(sudokuRect);
  }
}

std::string getText(const cv::Mat &inputImage) {
  SudokuImageParser parser(inputImage);
  return parser.parse();
}

cv::Mat BoardsFactory::parseImg(const std::string &filePath) {
  return cv::imread(filePath, cv::IMREAD_GRAYSCALE);
}

std::string BoardsFactory::getScreenShot() {
  std::cout << "Please capture the Sudoku board." << std::endl;
  screen_x = 0;
  screen_y = 0;

  system("screencapture -i sudoku.jpg");
  // calculate the screen size from the captured image
  cv::Mat screen = cv::imread("sudoku.jpg", cv::IMREAD_GRAYSCALE);
  screen_x = screen.cols;
  screen_y = screen.rows;
  cell_size = static_cast<int>(screen_x / 9);
  std::cout << "screen_x: " << screen_x << " screen_y: " << screen_y
            << " cell_size: " << cell_size << std::endl;
  return "sudoku.jpg";
}

std::string BoardsFactory::exec(const char* cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  std::cout<<"result: "<<result<<std::endl;
  return result;
}

void BoardsFactory::parseCells(const cv::Mat &inputImage, int board[9][9]) {
  tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
  if (api->Init(NULL, "eng")) {
    fprintf(stderr, "Could not initialize tesseract.\n");
    exit(1);
  }
  api->SetVariable("tessedit_char_whitelist", "123456789");
  int cellWidth = inputImage.cols / 9;
  int cellHeight = inputImage.rows / 9;
  int padding = 5; // Adjust this value as needed

  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      cv::Rect cellRect(j * cellWidth + padding, i * cellHeight + padding,
                        cellWidth - 2 * padding, cellHeight - 2 * padding);
      cv::Mat cellImage = inputImage(cellRect);

      // Convert the cell image to Pix
      Pix *cellPix = mat8ToPix(&cellImage);

      // Set the image for tesseract to the cell image
      api->SetImage(cellPix);

      // Get OCR result
      char *outText = api->GetUTF8Text();

      // If the cell is empty, assign it a value of 0
      if (outText[0] == '\0') {
        board[i][j] = 0;
      } else {
        board[i][j] = atoi(outText);
      }

      // Clean up
      delete[] outText;
      pixDestroy(&cellPix);
    }
  }
  // Destroy used object and release memory
  api->End();
}

void BoardsFactory::rotateBoard() {
  std::vector<std::vector<char>> rotatedBoard(9, std::vector<char>(9));
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      rotatedBoard[j][8 - i] = board[i][j];
    }
  }
  board = rotatedBoard;
}

void BoardsFactory::mirrorBoard() {
  std::vector<std::vector<char>> mirroredBoard(9, std::vector<char>(9));
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      mirroredBoard[i][8 - j] = board[i][j];
    }
  }
  board = mirroredBoard;
}

void BoardsFactory::rotate_CounterClockwise() {
  rotateBoard();
  mirrorBoard();
}

std::vector<std::vector<char>> BoardsFactory::getEmptyBoard() {
  return std::vector<std::vector<char>>(9, std::vector<char>(9, '.'));
}
BoardsFactory::BoardsFactory() {
  auto path = getScreenShot();
  inputImage_path = path;
  setBoard();
}

int BoardsFactory::getScreen_x() { return screen_x; }
int BoardsFactory::getScreen_y() { return screen_y; }
int BoardsFactory::getCell_size() { return cell_size; }

void BoardsFactory::setBoard(){
int int_board[9][9] = {0};
  cv::Mat inputImage = parseImg(inputImage_path);
  extractBoard(inputImage);
  parseCells(inputImage, int_board);
  board = itochar(int_board);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      std::cout << board[i][j] << " ";
    }
    std::cout << std::endl;
  }
  rotate_CounterClockwise();
}

// check for image path in the main function
BoardsFactory::BoardsFactory(const std::string &inputImage_path) : inputImage_path(inputImage_path) {
  setBoard();
}

std::vector<std::vector<char>> BoardsFactory::getBoard() { return board; }

// int main() {
//   std::string inputImage_path = "sudoku.jpg";
//   auto board = boardsFactory(inputImage_path);
//     for (int i = 0; i < 9; i++) {
//     for (int j = 0; j < 9; j++) {
//       std::cout << board[i][j] << " ";
//     }
//     std::cout << std::endl;
//   return 0;
// }
// }