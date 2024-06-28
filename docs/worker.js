importScripts('https://cdn.jsdelivr.net/npm/tesseract.js@4/dist/tesseract.min.js');

self.onmessage = async function(e) {
  const { width, height, imageData, row, col } = e.data;
  const result = await extractSudokuNumber(width, height, imageData);
  self.postMessage({ result });
};
async function extractSudokuNumber(cellWidth, cellHeight, imageData) {
  const cellImageData = new ImageData(new Uint8ClampedArray(imageData), cellWidth, cellHeight);


  // Convert ImageData to Blob as Tesseract.js can work with Blob for recognition
  const blob = await new Promise(resolve => {
    const canvas = new OffscreenCanvas(cellWidth, cellHeight);
const ctx = canvas.getContext('2d', { alpha: false, desynchronized: true, willReadFrequently: false });
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
