  document.getElementById('help-btn').addEventListener('click', function () {
            var tutorialPopup = document.getElementById('tutorial-popup');
            tutorialPopup.style.display = 'block'; // Use block if flex doesn't work as expected

            tutorialPopup.style.zIndex = '1000'; // Ensure it's above other content

            showStep(1);
        });

        document.querySelectorAll('.next-btn').forEach(btn => {
            btn.addEventListener('click', function () {
                const currentStep = this.parentElement.getAttribute('data-step');
                showStep(parseInt(currentStep) + 1);
            });
        });


        document.querySelectorAll('.copy-btn').forEach(btn => {
            btn.addEventListener('click', function () {
                const text = this.getAttribute('data-text');
                navigator.clipboard.writeText(text).then(() => {
                    alert('Copied to clipboard');
                });
            });
        });

        function showStep(step) {
            document.querySelectorAll('.tutorial-step').forEach(stepEl => {
                stepEl.style.display = 'none';
            });
            document.querySelector(`.tutorial-step[data-step="${step}"]`).style.display = 'block';
        }

        // after the last step, hide the popup - even if clicking on the overlay or copying the text
        document.getElementById('tutorial-popup').addEventListener('click', function (e) {
            if (e.target === this) {
                this.style.display = 'none';
            }
        });

        // if copied text from one of the examples, hide the popup and mark the #input-board as focused
        document.querySelectorAll('.copy-btn').forEach(btn => {
            btn.addEventListener('click', function () {
                document.getElementById('tutorial-popup').style.display = 'none';
                document.getElementById('input-board').setAttribute('placeholder', 'Paste the copied board here...');

            });
        });