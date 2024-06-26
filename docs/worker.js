// Global storage for debug images
const debugImages = [];

importScripts('https://cdn.jsdelivr.net/npm/tesseract.js@4/dist/tesseract.min.js');

self.onmessage = async function(e) {
  const { width, height, imageData, row, col } = e.data;
  const result = await extractSudokuNumber(width, height, imageData, row, col);
  self.postMessage({ result });
};

async function extractSudokuNumber(cellWidth, cellHeight, imageData, row, col) {
  const cellImageData = new ImageData(new Uint8ClampedArray(imageData), cellWidth, cellHeight);

  // Check for black pixels in the center of the cell
  if (!hasBlackPixelsInSampledLines(cellImageData)) {
    console.log('No black pixels found in cell at row:', row, 'col:', col);
    return '.'; // Return '.' immediately if no black pixels are found
  }

  // Save the cell image data for debugging
  debugImages.push({ row, col, imageData: cellImageData });

  // Convert ImageData to Blob as Tesseract.js can work with Blob for recognition
  const blob = await new Promise(resolve => {
    const canvas = new OffscreenCanvas(cellWidth, cellHeight);
    const ctx = canvas.getContext('2d');
    ctx.putImageData(cellImageData, 0, 0);
    canvas.convertToBlob().then(resolve);
  });

  const cellImageSrc = URL.createObjectURL(blob);

  const { data: { text } } = await Tesseract.recognize(cellImageSrc, 'eng', {
    tessedit_char_whitelist: '123456789',
    tessedit_pageseg_mode: Tesseract.PSM.SINGLE_CHAR
  });

  URL.revokeObjectURL(cellImageSrc);

  const digit = text.trim();
  const validDigit = digit.length === 1 && '123456789'.includes(digit) ? digit : '.';
  console.log('Extracted Digit: ', validDigit, 'at row:', row, 'col:', col);
  return validDigit;
}

// Existing functions remain unchanged...
function hasBlackPixelsInSampledLines(imageData) {
  const { width, height, data } = imageData;

  // Define lines to sample: 25% and 75% for both horizontal and vertical
  const rowsToCheck = [Math.floor(height * 0.25), Math.floor(height * 0.75)];
  const colsToCheck = [Math.floor(width * 0.25), Math.floor(width * 0.75)];

  // Check horizontal lines
  for (let y of rowsToCheck) {
    for (let x = 0; x < width; x++) {
      if (isBlackPixel(data, y, x, width)) return true;
    }
  }

  // Check vertical lines
  for (let x of colsToCheck) {
    for (let y = 0; y < height; y++) {
      if (isBlackPixel(data, y, x, width)) return true;
    }
  }

  return false; // No black pixels found in sampled lines
}

// Helper function to check if a pixel is black
function isBlackPixel(data, y, x, width) {
  const index = (y * width + x) * 4;
  return data[index] === 0 && data[index + 1] === 0 && data[index + 2] === 0 && data[index + 3] === 255;
}