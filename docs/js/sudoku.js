const sudokuBoard = document.getElementById('sudoku-board');
const initialBoard = '.................................................................................';

// Initialize empty Sudoku board
const board = Array.from({ length: 9 }, () => Array(9).fill('.'));

// Create the grid elements with spinners
for (let row = 0; row < 9; row++) {
    const rowElement = document.createElement('tr');
    // Add a class to the rowElement for the third and sixth rows
    if (row === 2 || row === 5) {
        rowElement.classList.add('section-border');
    }
    for (let col = 0; col < 9; col++) {
        const cellElement = document.createElement('td');
        const spinnerElement = document.createElement('div');
        spinnerElement.className = 'spinner';
        spinnerElement.dataset.row = row;
        spinnerElement.dataset.col = col;

        const wheelElement = document.createElement('div');
        wheelElement.className = 'wheel';
        wheelElement.innerHTML = Array.from({ length: 9 }, (_, k) => `<div class="number">${k + 1}</div>`).join('');

        spinnerElement.appendChild(wheelElement);
        cellElement.appendChild(spinnerElement);
        rowElement.appendChild(cellElement);
    }
    sudokuBoard.appendChild(rowElement);
}

    const digitTransforms = { 9: -200, 8: -150, 7: -100, 6: -50, 5: 0, 4: 50, 3: 100, 2: 150, 1: 200, '': -250 };
    let startY = 0;
    let startTransform = 0;
    let lastMoveTime = Date.now();
    let lastY = 0;
    let velocity = 0;

    function startSpin(event, cell) {
        event.preventDefault();
        const wheel = cell.querySelector('.wheel');
        startY = event.touches ? event.touches[0].clientY : event.clientY;
        lastY = startY;
        const transformMatrix = window.getComputedStyle(wheel).transform;
        startTransform = transformMatrix !== 'none' ? parseInt(transformMatrix.split(',')[5].trim()) : 0;
        document.onmousemove = document.ontouchmove = (e) => spinMove(e, wheel);
        document.onmouseup = document.ontouchend = () => endSpin(wheel, cell);
    }

// Adjust the inertiaFactor to increase responsiveness
const inertiaFactor = 0.1; // Increased from 0.1 to 0.5

function spinMove(event, wheel) {
    const currentY = event.touches ? event.touches[0].clientY : event.clientY;
    const deltaY = (currentY - startY); // Multiply the difference by 2 to amplify the effect
    const now = Date.now();
    // Modify the velocity calculation to increase sensitivity to small movements
    velocity = ((currentY - lastY)) / (now - lastMoveTime); // Multiply the difference by 2 to amplify the effect
    lastMoveTime = now;
    lastY = currentY;
    wheel.style.transform = `translateY(${startTransform + deltaY}px)`;
}

    function endSpin(wheel, cell) {
        document.onmousemove = document.ontouchmove = null;
        document.onmouseup = document.ontouchend = null;
        let currentTransform = parseInt(wheel.style.transform.split('(')[1].split('px')[0]);
        let endTransform = calculateEndTransform(currentTransform, velocity);
        let closestDigit = findClosestDigit(endTransform);
        wheel.style.transform = `translateY(${digitTransforms[closestDigit]}px)`;
        cell.dataset.value = closestDigit;
        synchronizeInputBoard();
    }

    function calculateEndTransform(currentTransform, velocity) {
        let estimatedEndTransform = currentTransform + (velocity * inertiaFactor);
        let closestDigit = findClosestDigit(estimatedEndTransform);
        return digitTransforms[closestDigit];
    }

    function findClosestDigit(estimatedEndTransform) {
    /* if currentTransform is 0, then the closest digit is 1 from up and 9 from down */

        let closestDigit = 1;
        let minDifference = Math.abs(estimatedEndTransform - digitTransforms[1]);
        for (let digit in digitTransforms) {
            let difference = Math.abs(estimatedEndTransform - digitTransforms[digit]);
            if (difference < minDifference) {
                minDifference = difference;
                closestDigit = digit;
            }
        }
        return closestDigit;
    }

    // Function to convert 2D array to board string
    function arrayToBoardString(array) {
        // Change 0 to . if there is any
        array = array.map(row => row.map(cell => cell === 0 ? '.' : cell));
        return array.flat().join('');
    }

    // Function to get the current cell position
    function getCurrentCellPosition() {
        const cell = document.activeElement;
        return {
            row: Number(cell.dataset.row),
            col: Number(cell.dataset.col)
        };
    }

    // Function to set cell value
    function setCellValue(row, col, value) {
        const cell = document.querySelector(`.spinner[data-row='${row}'][data-col='${col}']`);
        cell.dataset.value = value;
        const wheel = cell.querySelector('.wheel');
        wheel.style.transform = value ? `translateY(${digitTransforms[value]}px)` : '';
        synchronizeInputBoard();
    }

    // Function to distribute text across cells
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

    // Function to synchronize input board
    function synchronizeInputBoard() {
        const inputBoard = document.getElementById('input-board');
        let boardStr = '';
        for (let row = 0; row < 9; row++) {
            for (let col = 0; col < 9; col++) {
                const cell = document.querySelector(`.spinner[data-row='${row}'][data-col='${col}']`);
                boardStr += cell.dataset.value || '.';
            }
        }
        inputBoard.value = boardStr;
    }
function updateBoard(boardStr) {
    document.getElementById('input-board').value = boardStr;
    let index = 0;
    for (let row = 0; row < 9; row++) {
        for (let col = 0; col < 9; col++) {
            const cell = document.querySelector(`.spinner[data-row='${row}'][data-col='${col}']`);
            const value = boardStr[index];
            if (value === '.') {
                cell.dataset.value = '';
                cell.querySelector('.wheel').style.transform = 'translateY(-250px)'; // Ensure the spinner is reset
            } else {
                cell.dataset.value = value;
                cell.querySelector('.wheel').style.transform = `translateY(${digitTransforms[value]}px)`;
            }
            index++;
        }
    }
}

    // Event listener for pasting text
    sudokuBoard.addEventListener('paste', function (e) {
        e.preventDefault();
        const pastedText = (e.clipboardData || window.clipboardData).getData('text');
        distributeTextAcrossCells(pastedText, 'sudoku-board');
    });

    // Add the animation back to the wheel
    document.querySelectorAll('.wheel').forEach(wheel => {
        wheel.style.transition = 'transform 0.5s';
    });

    // Attach the startSpin function to each spinner element
    document.querySelectorAll('.spinner').forEach(spinner => {
        spinner.onmousedown = (event) => startSpin(event, spinner);
        spinner.ontouchstart = (event) => startSpin(event, spinner);
        spinner.onwheel = (event) => {
            event.preventDefault();
            const delta = Math.sign(event.deltaY);
            const wheel = spinner.querySelector('.wheel');
            const currentTransform = parseInt(wheel.style.transform.split('(')[1].split('px')[0]);
            let newTransform = currentTransform + delta * 50; // Adjust the number to control the scrolling speed
            let closestDigit = findClosestDigit(newTransform);
            wheel.style.transform = `translateY(${digitTransforms[closestDigit]}px)`;
            spinner.dataset.value = closestDigit;
            synchronizeInputBoard();
        };
    });

    // Add arrow keys support
    document.addEventListener('keydown', function (e) {
        if (e.key.startsWith('Arrow')) {
            e.preventDefault();
            const { row, col } = getCurrentCellPosition();
            switch (e.key) {
                case 'ArrowUp':
                    if (row > 0) document.querySelector(`.spinner[data-row='${row - 1}'][data-col='${col}']`).focus();
                    break;
                case 'ArrowDown':
                    if (row < 8) document.querySelector(`.spinner[data-row='${row + 1}'][data-col='${col}']`).focus();
                    break;
                case 'ArrowLeft':
                    if (col > 0) document.querySelector(`.spinner[data-row='${row}'][data-col='${col - 1}']`).focus();
                    break;
                case 'ArrowRight':
                    if (col < 8) document.querySelector(`.spinner[data-row='${row}'][data-col='${col + 1}']`).focus();
                    break;
            }
        }
    });

document.addEventListener('DOMContentLoaded', () => {
    // Move all spinners to blank state
    document.querySelectorAll('.spinner').forEach(spinner => {
        spinner.dataset.value = '';
        spinner.querySelector('.wheel').style.transform = '';
    });
    updateBoard(initialBoard);
});


    // Function to move to the next cell
    function moveToNextCell(row, col) {
        let nextCol = col;
        let nextRow = row;

        do {
            nextCol++;
            if (nextCol > 8) {
                nextCol = 0;
                nextRow++;
            }
            if (nextRow >= 9) {
                return;
            }
            var nextCell = document.querySelector(`.spinner[data-row='${nextRow}'][data-col='${nextCol}']`);
        } while (nextCell && nextCell.dataset.value);

        if (nextCell) {
            nextCell.focus();
        }
    }

    // Function to move to the previous cell
    function moveToPreviousCell(row, col) {
        let prevCol = col - 1;
        let prevRow = row;
        if (prevCol < 0) {
            prevCol = 8;
            prevRow--;
        }
        if (prevRow >= 0) {
            const prevCell = document.querySelector(`.spinner[data-row='${prevRow}'][data-col='${prevCol}']`);
            if (prevCell) {
                prevCell.focus();
            }
        }
    }
