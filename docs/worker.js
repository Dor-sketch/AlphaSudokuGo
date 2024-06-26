importScripts('https://cdn.jsdelivr.net/npm/tesseract.js@4/dist/tesseract.min.js');

self.onmessage = async function(e) {
  const { width, height, imageData, row, col } = e.data;
  const result = await extractSudokuNumber(width, height, imageData);
  self.postMessage({ result });
};
async function extractSudokuNumber(cellWidth, cellHeight, imageData) {
  const cellImageData = new ImageData(new Uint8ClampedArray(imageData), cellWidth, cellHeight);

  // Check for black pixels in the center of the cell
  if (!hasBlackPixelsInCenter(cellImageData)) {
    return '.'; // Return '.' immediately if no black pixels are found
  }

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
  console.log('Extracted Digit:', validDigit);
  return validDigit;
}

function hasBlackPixelsInCenter(imageData) {
  const { width, height, data } = imageData;
  const centerX = Math.floor(width / 2);
  const centerY = Math.floor(height / 2);
  const radius = Math.floor(Math.min(width, height) / 4); // Check a central square of the cell

  for (let y = centerY - radius; y <= centerY + radius; y++) {
    for (let x = centerX - radius; x <= centerX + radius; x++) {
      const index = (y * width + x) * 4;
      if (data[index] === 0 && data[index + 1] === 0 && data[index + 2] === 0 && data[index + 3] === 255) {
        // Found a black pixel (RGBA)
        return true;
      }
    }
  }
  return false; // No black pixels found
}