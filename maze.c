#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <emscripten/emscripten.h>

#define WIDTH 20
#define HEIGHT 20

int maze[HEIGHT][WIDTH];

EMSCRIPTEN_KEEPALIVE
void generateMaze() {
    srand(time(NULL));
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            maze[y][x] = rand() % 2;
        }
    }
    maze[0][0] = 0;
    maze[HEIGHT - 1][WIDTH - 1] = 0;
}

EMSCRIPTEN_KEEPALIVE
int* getMaze() {
    return &maze[0][0];
}