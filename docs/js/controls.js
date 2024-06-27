

function arrayToBoardString(array) {
    // change 0 to . if there is any
    array = array.map(row => row.map(cell => cell === 0 ? '.' : cell));
    return array.flat().join('');
}

        sudokuBoard.addEventListener('paste', function (e) {
            e.preventDefault();
            const pastedText = (e.clipboardData || window.clipboardData).getData('text');
            console.log('Pasted on sudoku-board:', pastedText);
            distributeTextAcrossCells(pastedText, 'sudoku-board');
        });

        function getCurrentCellPosition() {
            const cell = document.activeElement;
            return {
                row: Number(cell.dataset.row),
                col: Number(cell.dataset.col)
            };
        }

        function setCellValue(row, col, value) {
            const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
            cell.textContent = value;
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


        function updateBoard(boardStr) {
            inputBoard.value = boardStr;
            let index = 0;
            for (let row = 0; row < 9; row++) {
                for (let col = 0; col < 9; col++) {
                    const cell = document.querySelector(`.cell[data-row='${row}'][data-col='${col}']`);
                    cell.textContent = boardStr[index] === '.' ? '' : boardStr[index];
                    index++;
                }
            }
        }

        function navigateUsingArrowKeys(key, row, col) {
            switch (key) {
                case 'ArrowUp':
                    if (row > 0) document.querySelector(`.cell[data-row='${row - 1}'][data-col='${col}']`).focus();
                    break;
                case 'ArrowDown':
                    if (row < 8) document.querySelector(`.cell[data-row='${row + 1}'][data-col='${col}']`).focus();
                    break;
                case 'ArrowLeft':
                    if (col > 0) document.querySelector(`.cell[data-row='${row}'][data-col='${col - 1}']`).focus();
                    break;
                case 'ArrowRight':
                    if (col < 8) document.querySelector(`.cell[data-row='${row}'][data-col='${col + 1}']`).focus();
                    break;
            }
        }

        document.addEventListener('keydown', function (e) {
            if (e.key === ' ' && !e.repeat) {
                e.preventDefault();
                document.addEventListener('keydown', handleSpaceNavigation);
            } else if (e.key === 'Escape') {
                document.removeEventListener('keydown', handleSpaceNavigation);
            }
        });

        document.addEventListener('keyup', function (e) {
            if (e.key === ' ') {
                document.removeEventListener('keydown', handleSpaceNavigation);
            }
        });

        function handleSpaceNavigation(e) {
            const { row, col } = getCurrentCellPosition();
            if (e.key.startsWith('Arrow')) {
                navigateUsingArrowKeys(e.key, row, col);
            }
        }
