

const fileInput = document.getElementById('fileInput');
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const inputBoard = document.getElementById('input-board');
const processedImageElement = document.getElementById('processedImage');
function processImage(src) {
    return new Promise((resolve, reject) => {
        if (!src) {
            alert("Please upload an image first.");
            reject(new Error("No image source provided."));
            return;
        }

        const img = new Image();
        img.onload = async function () {
            // Resize the image to make processing faster
            const MAX_WIDTH = 500;
            const scale = MAX_WIDTH / img.width;
            const scaledHeight = img.height * scale;

            canvas.width = MAX_WIDTH;
            canvas.height = scaledHeight;
            ctx.drawImage(img, 0, 0, MAX_WIDTH, scaledHeight);

            // Binarize the image
            const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
            const data = imageData.data;
            for (let i = 0; i < data.length; i += 4) {
                const avg = data[i]; // Since the image is greyscale, we can just use the red channel
                data[i] = avg < 128 ? 0 : 255;
                data[i + 1] = data[i];
                data[i + 2] = data[i];
            }
            ctx.putImageData(imageData, 0, 0);
            const processedImageSrc = canvas.toDataURL();
            processedImageElement.src = processedImageSrc;

            try {
                const boardStr = await extractSudokuBoard(ctx, canvas.width, canvas.height);
                console.log(boardStr);
                inputBoard.value = boardStr;

                if (boardStr.length !== 81) {
                    resultElement.value += "\n\nFailed to parse Sudoku board. Please try again with a clearer image.";
                }
                resolve(); // Resolve the promise after all processing is done
            } catch (error) {
                reject(error); // Reject the promise if there's an error
            }
        };
        img.src = src;
    });
}function progressCallback(progress) {
    document.querySelector('.processingMessage').textContent = `Extraction progress: ${progress.toFixed(2)}%`;
}

async function extractSudokuBoard(ctx, width, height) {
    const cellWidth = width / 9;
    const cellHeight = height / 9;
    let boardStr = '';
    const totalCells = 9 * 9; // Total number of cells in a Sudoku board
    const ocrPromises = [];
    let cellsProcessed = 0; // Track the number of cells processed

    // Create a single canvas for reuse
    const cellCanvas = document.createElement('canvas');
    const cellCtx = cellCanvas.getContext('2d');
    cellCanvas.width = cellWidth;
    cellCanvas.height = cellHeight;

    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            // Reuse the canvas for each cell
            cellCtx.drawImage(ctx.canvas, col * cellWidth, row * cellHeight, cellWidth, cellHeight, 0, 0, cellWidth, cellHeight);
            const cellImageSrc = cellCanvas.toDataURL();

            // Push OCR promise to the array
            ocrPromises.push(Tesseract.recognize(cellImageSrc, 'eng', {
                tessedit_char_whitelist: '123456789',
                tessedit_pageseg_mode: Tesseract.PSM.SINGLE_CHAR
            }).then(({ data: { text } }) => {
                const digit = text.trim();
                cellsProcessed++;
                const progressPercent = (cellsProcessed / totalCells) * 100;
                progressCallback(progressPercent); // Update progress as each OCR operation completes
                return digit.length === 1 && '123456789'.includes(digit) ? digit : '.';
            }));
        }
    }

    // Wait for all OCR promises to resolve and update the board string
    const ocrResults = await Promise.all(ocrPromises);
    boardStr = ocrResults.join('');

    return boardStr;
}