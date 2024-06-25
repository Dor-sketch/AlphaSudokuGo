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

            const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
            const data = imageData.data;
            for (let i = 0; i < data.length; i += 4) {
                const avg = data[i];
                data[i] = avg < 128 ? 0 : 255;
                data[i + 1] = data[i];
                data[i + 2] = data[i];
            }
            ctx.putImageData(imageData, 0, 0);
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
