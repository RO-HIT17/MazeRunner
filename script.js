const canvas = document.getElementById('mazeCanvas');
        const ctx = canvas.getContext('2d');

        let mazeSize = 20;
        let cellSize = canvas.width / mazeSize;

        let player = { x: 0, y: 0 };
        let Module;
        let mazeData;
        let size;

        let startPoint;
        let endPoint;

        let gameFinished = false;

        
        MazeModule().then(async (mod) => {
            Module = mod;
            generateMaze();

            
            document.getElementById('generateMaze').addEventListener('click', () => {
                generateMaze();
            });

            document.getElementById('solveMaze').addEventListener('click', async () => {
                await solveMaze();
            });

            document.getElementById('resetMaze').addEventListener('click', () => {
                resetMaze();
            });
        });

        function generateMaze() {
            document.getElementById('message').innerText = '';
            gameFinished = false;
            Module._initializeScore(1000);
            mazeSize = parseInt(document.getElementById('sizeInput').value);
            if (mazeSize < 5) mazeSize = 5;
            if (mazeSize > 50) mazeSize = 50;
            cellSize = canvas.width / mazeSize;

            Module._generateMaze(mazeSize);
            const mazePtr = Module._getMaze();
            size = Module._getMazeSize();

            mazeData = new Int32Array(Module.HEAP32.buffer, mazePtr, size * size * 4);

            
            startPoint = {
                x: Module._getStartX(),
                y: Module._getStartY()
            };

            endPoint = {
                x: Module._getEndX(),
                y: Module._getEndY()
            };

            player = { x: startPoint.x, y: startPoint.y };
            drawMaze();
            updateScoreDisplay();
        }

        function updateScoreDisplay() {
            const currentScore = Module._getCurrentScore();
            document.getElementById('scoreDisplay').innerText = `Score: ${currentScore}`;
        }

        function decreaseScore(amount) {
            Module._decreaseScore(amount);
            updateScoreDisplay();
        }
        
        function displayScoreHistory() {
            
            const lengthPtr = Module._malloc(4); 
        
            const scoresPtr = Module._getScoreHistory(lengthPtr);
            const length = Module.getValue(lengthPtr, 'i32');
        
            const scores = [];
        
            for (let i = 0; i < length; i++) {
                const score = Module.getValue(scoresPtr + i * 4, 'i32'); // Assuming 32-bit integers
                scores.push(score);
            }
        
            Module._free(scoresPtr);
            Module._free(lengthPtr);
        
            const scoreList = document.getElementById('scoreList');
            scoreList.innerHTML = ''; 
            scores.reverse().forEach((score, index) => {
                const listItem = document.createElement('li');
                listItem.innerText = `Move ${index + 1}: Score ${score}`;
                scoreList.appendChild(listItem);
            });

            document.getElementById('scoreHistoryModal').classList.add('active');
            document.getElementById('overlay').classList.add('active');
        }

        function closeScoreHistory() {
            document.getElementById('scoreHistoryModal').classList.remove('active');
            document.getElementById('overlay').classList.remove('active');
        }

        document.getElementById('closeScoreHistory').addEventListener('click', () => {
            document.getElementById('scoreHistory').style.display = 'none';
        });
        
        document.getElementById('showScoreHistory').addEventListener('click', displayScoreHistory);
        document.getElementById('closeScoreHistory').addEventListener('click', closeScoreHistory);
        document.getElementById('overlay').addEventListener('click', closeScoreHistory);

        function resetMaze() {
            document.getElementById('message').innerText = '';
            gameFinished = false;
            player = { x: startPoint.x, y: startPoint.y };
            drawMaze();
        }

        function drawMaze() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.strokeStyle = '#333';
            ctx.lineWidth = 2;

            for (let y = 0; y < size; y++) {
                for (let x = 0; x < size; x++) {
                    const baseIndex = (y * size + x) * 4;
                    const walls = mazeData.slice(baseIndex, baseIndex + 4);

                    const xPos = x * cellSize;
                    const yPos = y * cellSize;

                    
                    if (walls[0]) {
                        ctx.beginPath();
                        ctx.moveTo(xPos, yPos);
                        ctx.lineTo(xPos + cellSize, yPos);
                        ctx.stroke();
                    }
                    
                    if (walls[1]) {
                        ctx.beginPath();
                        ctx.moveTo(xPos + cellSize, yPos);
                        ctx.lineTo(xPos + cellSize, yPos + cellSize);
                        ctx.stroke();
                    }
                    
                    if (walls[2]) {
                        ctx.beginPath();
                        ctx.moveTo(xPos + cellSize, yPos + cellSize);
                        ctx.lineTo(xPos, yPos + cellSize);
                        ctx.stroke();
                    }
                    
                    if (walls[3]) {
                        ctx.beginPath();
                        ctx.moveTo(xPos, yPos + cellSize);
                        ctx.lineTo(xPos, yPos);
                        ctx.stroke();
                    }
                }
            }

            
            ctx.fillStyle = 'blue';
            ctx.fillRect(
                endPoint.x * cellSize + cellSize * 0.25,
                endPoint.y * cellSize + cellSize * 0.25,
                cellSize * 0.5,
                cellSize * 0.5
            );

            
            ctx.fillStyle = 'red';
            ctx.beginPath();
            ctx.arc(
                player.x * cellSize + cellSize / 2,
                player.y * cellSize + cellSize / 2,
                cellSize * 0.3,
                0,
                2 * Math.PI
            );
            ctx.fill();
        }

        function sleep(ms) {
            return new Promise(resolve => setTimeout(resolve, ms));
        }

        async function solveMaze() {
            const algorithm = document.getElementById('algorithmSelect').value;
            if (algorithm === 'DFS') {
                Module._solveMazeDFS();
            } else if (algorithm === 'BFS') {
                Module._solveMazeBFS();
            }
            else if (algorithm === 'Dijkstra') {
                Module._solveMazeDijkstra();
            }

            await animateSearchAndPath();
        }

        async function animateSearchAndPath() {
            
            const explorationPtr = Module._getExplorationSteps();
            const explorationCount = Module._getExplorationStepCount();
            const explorationData = new Int32Array(Module.HEAP32.buffer, explorationPtr, explorationCount * 2);
        
            const pathPtr = Module._getPath();
            const pathLength = Module._getPathLength();
            const pathData = new Int32Array(Module.HEAP32.buffer, pathPtr, pathLength * 2);
        
            
            const algorithm = document.getElementById('algorithmSelect').value;
        
            
            let exploredColor = 'rgba(173, 216, 230, 0.5)'; 
            let pathColor = 'rgba(50, 205, 50, 0.6)';       
            let pathLineColor = 'green';
        
            if (algorithm === 'BFS') {
                exploredColor = 'rgba(255, 165, 0, 0.5)'; 
            } else if (algorithm === 'DFS') {
                exploredColor = 'rgba(138, 43, 226, 0.5)'; 
            } else if (algorithm === 'Dijkstra') {
                exploredColor = 'rgba(173, 216, 230, 0.5)'; 
            }
        
            
            const sleepDuration = 25; 
        
            
            for (let i = 0; i < explorationData.length / 2; i++) {
                drawMaze();
        
                
                ctx.fillStyle = exploredColor;
                for (let j = 0; j <= i; j++) {
                    const x = explorationData[j * 2];
                    const y = explorationData[j * 2 + 1];
                    ctx.fillRect(x * cellSize, y * cellSize, cellSize, cellSize);
                }
        
                await sleep(sleepDuration);
            }
        
            
            drawMaze();
        
            
            ctx.fillStyle = exploredColor;
            for (let i = 0; i < explorationData.length / 2; i++) {
                const x = explorationData[i * 2];
                const y = explorationData[i * 2 + 1];
                ctx.fillRect(x * cellSize, y * cellSize, cellSize, cellSize);
            }
        
            
            ctx.fillStyle = pathColor;
            for (let i = pathLength - 1; i >= 0; i--) {
                const x = pathData[i * 2];
                const y = pathData[i * 2 + 1];
                ctx.fillRect(x * cellSize, y * cellSize, cellSize, cellSize);
            }
        
            
            ctx.strokeStyle = pathLineColor;
            ctx.lineWidth = 3;
            ctx.beginPath();
            const startX = pathData[(pathLength - 1) * 2] * cellSize + cellSize / 2;
            const startY = pathData[(pathLength - 1) * 2 + 1] * cellSize + cellSize / 2;
            ctx.moveTo(startX, startY);
        
            for (let i = pathLength - 2; i >= 0; i--) {
                const x = pathData[i * 2] * cellSize + cellSize / 2;
                const y = pathData[i * 2 + 1] * cellSize + cellSize / 2;
                ctx.lineTo(x, y);
            }
            ctx.stroke();
        }
            function showMessage(message) {
                alert(message);
            }

            document.addEventListener('keydown', function(e) {
                if (gameFinished) return;

                let newX = player.x;
                let newY = player.y;


                if (e.key === 'ArrowUp') newY--;
                if (e.key === 'ArrowDown') newY++;
                if (e.key === 'ArrowLeft') newX--;
                if (e.key === 'ArrowRight') newX++;

                if (newX >= 0 && newX < mazeSize && newY >= 0 && newY < mazeSize) {
                    const baseIndex = (player.y * mazeSize + player.x) * 4;
                    const walls = mazeData.slice(baseIndex, baseIndex + 4);

                    if (e.key === 'ArrowUp' && !walls[0]) player.y--;
                    if (e.key === 'ArrowRight' && !walls[1]) player.x++;
                    if (e.key === 'ArrowDown' && !walls[2]) player.y++;
                    if (e.key === 'ArrowLeft' && !walls[3]) player.x--;

                    drawMaze();
                    decreaseScore(1);

                    const currentScore = Module._getCurrentScore();

                    if (player.x === endPoint.x && player.y === endPoint.y) {
                        gameFinished = true;
                        showMessage('Congratulations! You reached the destination!');
                        showMessage(`Your final score is: ${currentScore}`);
                    }

                    if (currentScore <= 0) {
                        gameFinished = true;
                        showMessage('Game Over! Your score reached zero.');
                    }
                }
            });
            
            document.getElementById('showScoreHistory').addEventListener('click', () => {
                displayScoreHistory();
            });

            async function solveMaze() {
                const algorithm = document.getElementById('algorithmSelect').value;
                if (algorithm === 'DFS') {
                    Module._solveMazeDFS();
                } else if (algorithm === 'BFS') {
                    Module._solveMazeBFS();
                } else if (algorithm === 'Dijkstra') {
                    Module._solveMazeDijkstra();
                }

                await animateSearchAndPath();
            }