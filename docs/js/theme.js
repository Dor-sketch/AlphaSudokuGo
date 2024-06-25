// Cache DOM references
const themeToggle = document.getElementById('theme-toggle');
const tutorialPopup = document.getElementById('tutorial-popup');

themeToggle.addEventListener('click', function () {
    this.classList.toggle('toggle-on');
    document.body.classList.toggle('dark-theme');
});

document.addEventListener('DOMContentLoaded', function () {
    // Use event delegation for close buttons
    document.body.addEventListener('click', function (e) {
        if (e.target.classList.contains('close-btn')) {
            tutorialPopup.style.display = 'none';
        }
    });
});