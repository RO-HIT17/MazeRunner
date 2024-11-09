#include <stdlib.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define MAX_SIZE 50

int mazeSize;
int maze[MAX_SIZE][MAX_SIZE][4]; // 0: Top, 1: Right, 2: Bottom, 3: Left
int visited[MAX_SIZE][MAX_SIZE];

void carvePassagesFrom(int cx, int cy) {
    int directions[4] = {0, 1, 2, 3};
    // Shuffle directions
    for (int i = 0; i < 4; i++) {
        int rnd = rand() % 4;
        int temp = directions[i];
        directions[i] = directions[rnd];
        directions[rnd] = temp;
    }

    visited[cy][cx] = 1;

    for (int i = 0; i < 4; i++) {
        int nx = cx;
        int ny = cy;

        switch (directions[i]) {
            case 0: ny = cy - 1; break; // Up
            case 1: nx = cx + 1; break; // Right
            case 2: ny = cy + 1; break; // Down
            case 3: nx = cx - 1; break; // Left
        }

        if (nx >= 0 && nx < mazeSize && ny >= 0 && ny < mazeSize && !visited[ny][nx]) {
            maze[cy][cx][directions[i]] = 0;
            maze[ny][nx][(directions[i] + 2) % 4] = 0;
            carvePassagesFrom(nx, ny);
        }
    }
}


EMSCRIPTEN_KEEPALIVE
void generateMaze(int size) {
    if (size > MAX_SIZE) size = MAX_SIZE;
    mazeSize = size;

    // Initialize maze and visited arrays
    for (int y = 0; y < mazeSize; y++) {
        for (int x = 0; x < mazeSize; x++) {
            maze[y][x][0] = 1; // Top wall
            maze[y][x][1] = 1; // Right wall
            maze[y][x][2] = 1; // Bottom wall
            maze[y][x][3] = 1; // Left wall
            visited[y][x] = 0;
        }
    }

    srand(time(NULL));

    // Start maze generation from (0, 0)
    carvePassagesFrom(0, 0);
}


EMSCRIPTEN_KEEPALIVE
int* getMaze() {
    return &maze[0][0][0];
}

EMSCRIPTEN_KEEPALIVE
int getMazeSize() {
    return mazeSize;
}