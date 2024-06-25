


        document.getElementById('theme-toggle').addEventListener('click', function () {
            this.classList.toggle('toggle-on');
            document.body.classList.toggle('dark-theme');
        });

        document.addEventListener('DOMContentLoaded', function () {
            document.querySelectorAll('.close-btn').forEach(function(button) {
                button.addEventListener('click', function () {
                    document.getElementById('tutorial-popup').style.display = 'none';
                });
            });
        });
