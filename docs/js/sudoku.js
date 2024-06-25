
const sudokuBoard = document.getElementById('sudoku-board');

// Initialize empty Sudoku board
const board = Array.from({ length: 9 }, () => Array(9).fill('.'));

// Create the grid elements
for (let row = 0; row < 9; row++) {
    for (let col = 0; col < 9; col++) {
        const cell = document.createElement('div');
        cell.className = 'cell';
        cell.contentEditable = true;
        cell.dataset.row = row;
        cell.dataset.col = col;

        // Add event listener for handling input
        cell.addEventListener('input', function (e) {
            const value = e.target.textContent.trim();
            if (value.match(/[1-9]/)) { // If a number between 1 and 9 is entered
                e.target.textContent = value; // Set the cell's content to the entered number
                moveToNextCell(row, col); // Move to the next cell
            } else {
                e.target.textContent = ''; // Clear the cell if the input is not a number
            }
            synchronizeInputBoard();
        });
        // Add event listener for handling keydown
        cell.addEventListener('keydown', function (e) {
            // Check if the key pressed is a number or navigation/functional key
            if ((e.key >= '0' && e.key <= '9') || e.key === 'Backspace' || e.key === 'Enter' || e.key === ' ') {
                e.preventDefault(); // Prevent the default action

                if (e.key === 'Backspace') {
                    e.target.textContent = ''; // Clear the cell
                    moveToPreviousCell(row, col); // Move to the previous cell
                } else if (e.key === ' ' || e.key === 'Enter') {
                    e.target.textContent = ''; // Clear the cell
                    moveToNextCell(row, col); // Move to the next cell
                } else {
                    // Ensure the cell is empty before setting a new number
                    e.target.textContent = ''; // Clear existing content
                    e.target.textContent = e.key; // Set new number
                    moveToNextCell(row, col); // Then move to the next cell
                }

                synchronizeInputBoard();
            }
        });
        sudokuBoard.appendChild(cell);
    }
}

function moveToPreviousCell(row, col) {
    let prevCol = col - 1;
    let prevRow = row;
    if (prevCol < 0) { // If the current cell is the first in its row, move to the last cell of the previous row
        prevCol = 8;
        prevRow--;
    }
    if (prevRow >= 0) { // If the previous row exists
        const prevCell = document.querySelector(`.cell[data-row='${prevRow}'][data-col='${prevCol}']`);
        if (prevCell) {
            prevCell.focus(); // Move focus to the previous cell
        }
    }
}

// Function to move focus to the next cell, skipping cells that already contain a number
function moveToNextCell(row, col) {
    let nextCol = col;
    let nextRow = row;

    do {
        nextCol++;
        if (nextCol > 8) { // If the current cell is the last in its row, move to the first cell of the next row
            nextCol = 0;
            nextRow++;
        }
        if (nextRow >= 9) { // If there are no more rows, stop the loop
            return;
        }

        var nextCell = document.querySelector(`.cell[data-row='${nextRow}'][data-col='${nextCol}']`);
    } while (nextCell && nextCell.value); // Continue if the next cell exists and has a value

    if (nextCell) {
        nextCell.focus(); // Move focus to the next empty cell
    }
}

// Function to synchronize the input board with the grid
function synchronizeInputBoard() {
    let boardStr = '';
    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
            boardStr += cell.textContent.trim() || '.';
        }
    }
    inputBoard.value = boardStr;
}

// Synchronize text input with board cells
inputBoard.addEventListener('input', function (e) {
    const value = e.target.value.trim();
    for (let i = 0; i < 81; i++) {
        const row = Math.floor(i / 9);
        const col = i % 9;
        const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
        cell.textContent = (value[i] && value[i].match(/[1-9]/)) ? value[i] : '';
    }
});

// Function to convert 2D array to board string
function arrayToBoardString(array) {
    // change 0 to . if there is any
    array = array.map(row => row.map(cell => cell === 0 ? '.' : cell));
    return array.flat().join('');
}

// Event listener for pasting on the sudoku-board
sudokuBoard.addEventListener('paste', function (e) {
    e.preventDefault(); // Prevent the default paste action
    const pastedText = (e.clipboardData || window.clipboardData).getData('text');
    console.log('Pasted on sudoku-board:', pastedText);
    distributeTextAcrossCells(pastedText, 'sudoku-board'); // Assuming this function handles the distribution of text across cells for the sudoku board
});

// Placeholder for the function to get the current cell's position
function getCurrentCellPosition() {
    const cell = document.activeElement;
    return {
        row: Number(cell.dataset.row),
        col: Number(cell.dataset.col)
    };
}

// Placeholder for the function to set the value of a cell
function setCellValue(row, col, value) {
    const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
    cell.textContent = value;
    // update the input board
    synchronizeInputBoard();
}

function distributeTextAcrossCells(text, target) {
    const splitText = text.split('');
    let { row, col } = getCurrentCellPosition();
    for (let i = 0; i < splitText.length; i++) {
        setCellValue(row, col, splitText[i]);
        if (target === 'sudoku-board') {
            moveToNextCell(row, col);
            ({ row, col } = getCurrentCellPosition());
        }
    }
}

inputBoard.addEventListener('paste', function (e) {
    e.preventDefault();
    const pastedText = (e.clipboardData || window.clipboardData).getData('text');
    try {
        let parsedArray;
        // Check if pastedText contains "."
        const containsDot = pastedText.includes('.');

        // Attempt to parse as JSON first
        try {
            parsedArray = JSON.parse(pastedText);
        } catch {
            // If JSON parsing fails, process as a plain string
            if (pastedText.length === 81) { // Ensure it's the right length for a Sudoku board
                parsedArray = [];
                for (let i = 0; i < 9; i++) {
                    let row;
                    if (containsDot) {
                        // If pastedText contains ".", split directly without converting "0" to "."
                        row = pastedText.slice(i * 9, (i + 1) * 9).split('').map(cell => cell === '.' ? 0 : Number(cell));
                    } else {
                        // If pastedText does not contain ".", proceed as before
                        row = pastedText.slice(i * 9, (i + 1) * 9).split('').map(Number);
                    }
                    parsedArray.push(row);
                }
            }
        }
        // Validate the parsed array
        if (Array.isArray(parsedArray) && parsedArray.length === 9 && parsedArray.every(row => Array.isArray(row) && row.length === 9)) {
            const boardString = arrayToBoardString(parsedArray);
            inputBoard.value = boardString;
            updateBoard(boardString);
        }
        // check if cant trim white spaces and commas
        else if (pastedText.length === 81) {
            const boardString = pastedText.replace(/[^0-9]/g, '');
            inputBoard.value = boardString;
            updateBoard(boardString);
        }

        else {
            console.error('Pasted content is not a valid Sudoku board.');
        }
    } catch (error) {
        console.error('Error parsing pasted content:', error);
    }
});

// Function to read board from DOM
function readBoard() {
    return inputBoard.value.trim();
}

// Function to update board in DOM
function updateBoard(boardStr) {
    let index = 0;
    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
            cell.textContent = boardStr[index] === '.' ? '' : boardStr[index];
            index++;
        }
    }
}

// Load the WebAssembly module
var Module = {
    onRuntimeInitialized: () => {
        console.log('WASM module loaded');
    }
};

function isValidPlacement(boardStr, row, col, num) {
    for (let i = 0; i < 9; i++) {
        if (boardStr[row * 9 + i] === num || boardStr[i * 9 + col] === num) {
            return false;
        }
    }
    const startRow = Math.floor(row / 3) * 3;
    const startCol = Math.floor(col / 3) * 3;
    for (let i = startRow; i < startRow + 3; i++) {
        for (let j = startCol; j < startCol + 3; j++) {
            if (boardStr[i * 9 + j] === num) {
                return false;
            }
        }
    }
    return true;
}

function isValidSudoku(boardStr) {
    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            if (boardStr[i * 9 + j] !== '.') {
                const num = boardStr[i * 9 + j];
                boardStr = boardStr.substr(0, i * 9 + j) + '.' + boardStr.substr(i * 9 + j + 1);
                if (!isValidPlacement(boardStr, i, j, num)) {
                    return false;
                }
                boardStr = boardStr.substr(0, i * 9 + j) + num + boardStr.substr(i * 9 + j + 1);
            }
        }
    }
    return true;
}



function celebrate() {
    const cells = document.querySelectorAll('.cell');
    cells.forEach(cell => {
        cell.style.backgroundColor = "#8E8E93"; // Adjusted to a lighter Apple-esque gray
    });
    setTimeout(() => {
        cells.forEach(cell => {
            cell.style.backgroundColor = '';
        });
    }, 2000);
}
