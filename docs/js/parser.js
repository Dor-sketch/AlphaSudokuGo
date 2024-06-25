const fileInput = document.getElementById('fileInput');
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const inputBoard = document.getElementById('input-board');
const processedImageElement = document.getElementById('processedImage');

document.getElementById('cancelCrop').addEventListener('click', function () {
    document.getElementById('confirmCrop').style.display = 'none';
    document.getElementById('cancelCrop').style.display = 'none';
    document.getElementById('imageToCrop').style.display = 'none';

    if (typeof cropper !== 'undefined') {
        cropper.destroy();
    }

    resetUI();
});

function resetUI() {
    document.getElementById('parser').style.display = 'none';
    document.getElementsByClassName('upload-btn')[0].style.display = 'block';
    document.getElementById('cropping-actions').style.display = 'none';
    document.getElementById('sudoku-board').style.display = 'grid';
    document.getElementById('processedImage').style.display = 'none';
    document.getElementById('processedImageContainer').style.display = 'none';
    document.getElementById('input-board').style.display = 'block';
    document.getElementsByClassName('solve-button')[0].style.display = 'block';
    document.body.classList.remove('dark-theme');
    document.body.style.opacity = 1;
}

async function handleFileSelection(files) {
    if (files.length > 0) {
        document.body.classList.toggle('dark-theme');
        document.getElementById('parser').style.display = 'block';
        document.getElementById('sudoku-board').style.display = 'none';
        document.getElementById('processedImage').style.display = 'block';
        document.getElementById('processedImageContainer').style.display = 'flex';
        document.getElementsByClassName('processingMessage')[0].style.display = 'block';
        document.getElementsByClassName('upload-btn')[0].style.display = 'none';
        document.getElementById('cropping-actions').style.display = 'block';
        document.getElementById('tutorial-popup').style.display = 'none';
        document.getElementById('input-board').style.display = 'none';
        document.getElementsByClassName('solve-button')[0].style.display = 'none';

        try {
            const img_src = await initiateCropping(files[0]);
            await processImage(img_src);
            alert("Processing and updating done!");
        } catch (error) {
            console.error("An error occurred:", error);
            alert("An error occurred. Please try again.");
        } finally {
            resetUI();
        }
    }
}

function processImage(src) {
    return new Promise((resolve, reject) => {
        if (!src) {
            alert("Please upload an image first.");
            reject(new Error("No image source provided."));
            return;
        }

        const img = new Image();
        img.onload = async function () {
            const MAX_WIDTH = 500;
            const scale = MAX_WIDTH / img.width;
            const scaledHeight = img.height * scale;

            canvas.width = MAX_WIDTH;
            canvas.height = scaledHeight;
            ctx.drawImage(img, 0, 0, MAX_WIDTH, scaledHeight);

            // Binarize and enhance the image
            const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
            const data = imageData.data;
            for (let i = 0; i < data.length; i += 4) {
                const avg = data[i] * 0.3 + data[i + 1] * 0.59 + data[i + 2] * 0.11; // Grayscale
                const binarized = avg < 128 ? 0 : 255;
                data[i] = binarized;
                data[i + 1] = binarized;
                data[i + 2] = binarized;
            }
            ctx.putImageData(imageData, 0, 0);

            // Apply a Gaussian blur to reduce noise
            const blurredImageData = applyGaussianBlur(ctx.getImageData(0, 0, canvas.width, canvas.height));
            ctx.putImageData(blurredImageData, 0, 0);

            const processedImageSrc = canvas.toDataURL();
            processedImageElement.src = processedImageSrc;

            try {
                const boardStr = await extractSudokuBoardParallel(ctx, canvas.width, canvas.height);
                inputBoard.value = boardStr;

                if (boardStr.length !== 81) {
                    alert("Failed to parse Sudoku board. Please try again with a clearer image.");
                }
                resolve();
            } catch (error) {
                reject(error);
            }
        };
        img.src = src;
    });
}

function applyGaussianBlur(imageData) {
    const kernel = [
        [1, 4, 7, 4, 1],
        [4, 16, 26, 16, 4],
        [7, 26, 41, 26, 7],
        [4, 16, 26, 16, 4],
        [1, 4, 7, 4, 1]
    ];
    const kernelSum = 273;
    const width = imageData.width;
    const height = imageData.height;
    const data = imageData.data;
    const result = new Uint8ClampedArray(data.length);

    for (let y = 2; y < height - 2; y++) {
        for (let x = 2; x < width - 2; x++) {
            let r = 0, g = 0, b = 0;
            for (let ky = -2; ky <= 2; ky++) {
                for (let kx = -2; kx <= 2; kx++) {
                    const pixelIndex = ((y + ky) * width + (x + kx)) * 4;
                    const weight = kernel[ky + 2][kx + 2];
                    r += data[pixelIndex] * weight;
                    g += data[pixelIndex + 1] * weight;
                    b += data[pixelIndex + 2] * weight;
                }
            }
            const resultIndex = (y * width + x) * 4;
            result[resultIndex] = r / kernelSum;
            result[resultIndex + 1] = g / kernelSum;
            result[resultIndex + 2] = b / kernelSum;
            result[resultIndex + 3] = data[resultIndex + 3]; // Alpha
        }
    }

    return new ImageData(result, width, height);
}

function progressCallback(progress) {
    document.querySelector('.processingMessage').textContent = `Extraction progress: ${progress.toFixed(2)}%`;
}

async function extractSudokuBoardParallel(ctx, width, height) {
    const cellWidth = width / 9;
    const cellHeight = height / 9;
    let boardStr = '';
    const totalCells = 9 * 9;

    const cellPromises = [];

    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            cellPromises.push(extractCell(ctx, col, row, cellWidth, cellHeight));
        }
    }

    const cellResults = await Promise.all(cellPromises);

    cellResults.forEach((digit, index) => {
        const row = Math.floor(index / 9);
        const col = index % 9;
        boardStr += digit.length === 1 && '123456789'.includes(digit) ? digit : '.';

        const cellsProcessed = row * 9 + col + 1;
        const progressPercent = (cellsProcessed / totalCells) * 100;
        progressCallback(progressPercent);
    });

    return boardStr;
}

function extractCell(ctx, col, row, cellWidth, cellHeight) {
    return new Promise((resolve) => {
        const cellCanvas = document.createElement('canvas');
        const cellCtx = cellCanvas.getContext('2d');
        cellCanvas.width = cellWidth;
        cellCanvas.height = cellHeight;
        cellCtx.drawImage(ctx.canvas, col * cellWidth, row * cellHeight, cellWidth, cellHeight, 0, 0, cellWidth, cellHeight);

        const cellImageSrc = cellCanvas.toDataURL();
        Tesseract.recognize(cellImageSrc, 'eng', {
            tessedit_char_whitelist: '123456789',
            tessedit_pageseg_mode: Tesseract.PSM.SINGLE_CHAR
        }).then(({ data: { text } }) => {
            const digit = text.trim();
            resolve(digit);
        });
    });
}
