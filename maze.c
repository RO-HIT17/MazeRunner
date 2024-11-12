#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include <limits.h>

#define MAX_SIZE 50

typedef struct Node {
    int x, y;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
} AdjList;

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point* data;
    int top;
    int capacity;
} Stack;

typedef struct {
    Point* data;
    int front, rear, size;
    int capacity;
} Queue;

typedef struct {
    Point* data;
    int* dist;
    int size;
    int capacity;
} MinHeap;


int mazeSize;
AdjList graph[MAX_SIZE][MAX_SIZE];
int visited[MAX_SIZE][MAX_SIZE];
Point path[MAX_SIZE * MAX_SIZE];
int pathLength;


Point explorationSteps[MAX_SIZE * MAX_SIZE];
int explorationStepCount;


Point startPoint;
Point endPoint;

void initGraph() {
    for (int y = 0; y < MAX_SIZE; y++) {
        for (int x = 0; x < MAX_SIZE; x++) {
            graph[y][x].head = NULL;
        }
    }
}

void addEdge(int x1, int y1, int x2, int y2) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->x = x2;
    newNode->y = y2;
    newNode->next = graph[y1][x1].head;
    graph[y1][x1].head = newNode;
}

void carvePassagesFrom(int cx, int cy) {
    int directions[4] = {0, 1, 2, 3};
    
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
            case 0: ny = cy - 1; break; 
            case 1: nx = cx + 1; break; 
            case 2: ny = cy + 1; break; 
            case 3: nx = cx - 1; break; 
        }

        if (nx >= 0 && nx < mazeSize && ny >= 0 && ny < mazeSize && !visited[ny][nx]) {
            addEdge(cx, cy, nx, ny);
            addEdge(nx, ny, cx, cy);
            carvePassagesFrom(nx, ny);
        }
    }
}

EMSCRIPTEN_KEEPALIVE
void generateMaze(int size) {
    if (size > MAX_SIZE) size = MAX_SIZE;
    mazeSize = size;

    initGraph();
    memset(visited, 0, sizeof(visited));

    srand(time(NULL));
    carvePassagesFrom(0, 0);


    startPoint.x = rand() % mazeSize;
    startPoint.y = rand() % mazeSize;

    do {
        endPoint.x = rand() % mazeSize;
        endPoint.y = rand() % mazeSize;
    } while (endPoint.x == startPoint.x && endPoint.y == startPoint.y);
}

EMSCRIPTEN_KEEPALIVE
int getStartX() {
    return startPoint.x;
}

EMSCRIPTEN_KEEPALIVE
int getStartY() {
    return startPoint.y;
}

EMSCRIPTEN_KEEPALIVE
int getEndX() {
    return endPoint.x;
}

EMSCRIPTEN_KEEPALIVE
int getEndY() {
    return endPoint.y;
}

EMSCRIPTEN_KEEPALIVE
int* getMaze() {
    static int maze[MAX_SIZE * MAX_SIZE * 4];
    int index = 0;
    for (int y = 0; y < mazeSize; y++) {
        for (int x = 0; x < mazeSize; x++) {
            maze[index] = 1;     
            maze[index + 1] = 1; 
            maze[index + 2] = 1; 
            maze[index + 3] = 1; 

            Node* node = graph[y][x].head;
            while (node != NULL) {
                if (node->x == x && node->y == y - 1) maze[index] = 0; // Top
                if (node->x == x + 1 && node->y == y) maze[index + 1] = 0; // Right
                if (node->x == x && node->y == y + 1) maze[index + 2] = 0; // Bottom
                if (node->x == x - 1 && node->y == y) maze[index + 3] = 0; // Left
                node = node->next;
            }
            index += 4;
        }
    }
    return maze;
}

EMSCRIPTEN_KEEPALIVE
int getMazeSize() {
    return mazeSize;
}

EMSCRIPTEN_KEEPALIVE
int* getExplorationSteps() {
    return (int*)explorationSteps;
}

EMSCRIPTEN_KEEPALIVE
int getExplorationStepCount() {
    return explorationStepCount;
}

EMSCRIPTEN_KEEPALIVE
int* getPath() {
    return (int*)path;
}

EMSCRIPTEN_KEEPALIVE
int getPathLength() {
    return pathLength;
}


Stack* createStack(int capacity) {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->data = (Point*)malloc(stack->capacity * sizeof(Point));
    return stack;
}

int isStackEmpty(Stack* stack) {
    return stack->top == -1;
}

void push(Stack* stack, Point item) {
    if (stack->top == stack->capacity - 1) return;
    stack->data[++stack->top] = item;
}

Point pop(Stack* stack) {
    if (isStackEmpty(stack)) {
        Point p = {-1, -1};
        return p;
    }
    return stack->data[stack->top--];
}

void freeStack(Stack* stack) {
    free(stack->data);
    free(stack);
}


Queue* createQueue(int capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->data = (Point*)malloc(queue->capacity * sizeof(Point));
    return queue;
}

int isQueueEmpty(Queue* queue) {
    return queue->size == 0;
}

void enqueue(Queue* queue, Point item) {
    if (queue->size == queue->capacity) return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->data[queue->rear] = item;
    queue->size++;
}

Point dequeue(Queue* queue) {
    if (isQueueEmpty(queue)) {
        Point p = {-1, -1};
        return p;
    }
    Point item = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

void freeQueue(Queue* queue) {
    free(queue->data);
    free(queue);
}


MinHeap* createMinHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = (Point*)malloc(sizeof(Point) * capacity);
    heap->dist = (int*)malloc(sizeof(int) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swap(MinHeap* heap, int i, int j) {
    Point tempPoint = heap->data[i];
    heap->data[i] = heap->data[j];
    heap->data[j] = tempPoint;

    int tempDist = heap->dist[i];
    heap->dist[i] = heap->dist[j];
    heap->dist[j] = tempDist;
}

void minHeapifyUp(MinHeap* heap, int idx) {
    while (idx > 0 && heap->dist[(idx - 1) / 2] > heap->dist[idx]) {
        swap(heap, idx, (idx - 1) / 2);
        idx = (idx - 1) / 2;
    }
}

void minHeapifyDown(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = idx * 2 + 1;
    int right = idx * 2 + 2;

    if (left < heap->size && heap->dist[left] < heap->dist[smallest])
        smallest = left;
    if (right < heap->size && heap->dist[right] < heap->dist[smallest])
        smallest = right;

    if (smallest != idx) {
        swap(heap, idx, smallest);
        minHeapifyDown(heap, smallest);
    }
}

void insertHeap(MinHeap* heap, Point p, int distance) {
    if (heap->size == heap->capacity) return;

    heap->data[heap->size] = p;
    heap->dist[heap->size] = distance;
    heap->size++;
    minHeapifyUp(heap, heap->size - 1);
}

Point extractMin(MinHeap* heap, int* distance) {
    if (heap->size == 0) {
        Point p = {-1, -1};
        *distance = INT_MAX;
        return p;
    }

    Point minPoint = heap->data[0];
    *distance = heap->dist[0];

    heap->data[0] = heap->data[heap->size - 1];
    heap->dist[0] = heap->dist[heap->size - 1];
    heap->size--;
    minHeapifyDown(heap, 0);

    return minPoint;
}

void freeMinHeap(MinHeap* heap) {
    free(heap->data);
    free(heap->dist);
    free(heap);
}

EMSCRIPTEN_KEEPALIVE
void solveMazeDFS() {
    Stack* stack = createStack(mazeSize * mazeSize);
    push(stack, startPoint);
    explorationStepCount = 0;
    memset(visited, 0, sizeof(visited));
    pathLength = 0;

    int previous[MAX_SIZE][MAX_SIZE][2];
    for (int y = 0; y < mazeSize; y++)
        for (int x = 0; x < mazeSize; x++)
            previous[y][x][0] = previous[y][x][1] = -1;

    while (!isStackEmpty(stack)) {
        Point p = pop(stack);
        int x = p.x;
        int y = p.y;

        if (x < 0 || x >= mazeSize || y < 0 || y >= mazeSize) continue;
        if (visited[y][x]) continue;
        visited[y][x] = 1;
        explorationSteps[explorationStepCount++] = p;

        if (x == endPoint.x && y == endPoint.y) {
            
            pathLength = 0;
            Point curr = p;
            while (curr.x != -1 && curr.y != -1) {
                path[pathLength++] = curr;
                int px = previous[curr.y][curr.x][0];
                int py = previous[curr.y][curr.x][1];
                curr.x = px;
                curr.y = py;
            }
            break;
        }

        Node* node = graph[y][x].head;
        while (node != NULL) {
            if (!visited[node->y][node->x]) {
                push(stack, (Point){node->x, node->y});
                previous[node->y][node->x][0] = x;  
                previous[node->y][node->x][1] = y;
            }
            node = node->next;
        }
    }

    freeStack(stack);
}


EMSCRIPTEN_KEEPALIVE
void solveMazeBFS() {
    Queue* queue = createQueue(mazeSize * mazeSize);
    enqueue(queue, startPoint);
    explorationStepCount = 0;
    memset(visited, 0, sizeof(visited));
    pathLength = 0;

    int previous[MAX_SIZE][MAX_SIZE][2]; 
    for (int y = 0; y < mazeSize; y++)
        for (int x = 0; x < mazeSize; x++)
            previous[y][x][0] = previous[y][x][1] = -1;

    while (!isQueueEmpty(queue)) {
        Point p = dequeue(queue);
        int x = p.x;
        int y = p.y;

        if (x < 0 || x >= mazeSize || y < 0 || y >= mazeSize) continue;
        if (visited[y][x]) continue;
        visited[y][x] = 1;
        explorationSteps[explorationStepCount++] = p;

        if (x == endPoint.x && y == endPoint.y) {
            
            pathLength = 0;
            Point curr = p;
            while (curr.x != -1 && curr.y != -1) {
                path[pathLength++] = curr;
                int px = previous[curr.y][curr.x][0];
                int py = previous[curr.y][curr.x][1];
                curr.x = px;
                curr.y = py;
            }
            break;
        }

        Node* node = graph[y][x].head;
        while (node != NULL) {
            if (!visited[node->y][node->x]) {
                enqueue(queue, (Point){node->x, node->y});
                previous[node->y][node->x][0] = x;
                previous[node->y][node->x][1] = y;
            }
            node = node->next;
        }
    }

    freeQueue(queue);
}


EMSCRIPTEN_KEEPALIVE
void solveMazeDijkstra() {
    int dist[MAX_SIZE][MAX_SIZE];
    int previous[MAX_SIZE][MAX_SIZE][2];
    int inSet[MAX_SIZE][MAX_SIZE];

    explorationStepCount = 0;  
    pathLength = 0;           
    memset(visited, 0, sizeof(visited)); 

    for (int y = 0; y < mazeSize; y++) {
        for (int x = 0; x < mazeSize; x++) {
            dist[y][x] = INT_MAX;
            previous[y][x][0] = previous[y][x][1] = -1;
            inSet[y][x] = 0; 
        }
    }

    MinHeap* heap = createMinHeap(mazeSize * mazeSize);
    dist[startPoint.y][startPoint.x] = 0;
    insertHeap(heap, startPoint, 0);

    while (heap->size > 0) {
        int currentDist;
        Point u = extractMin(heap, &currentDist);

        if (inSet[u.y][u.x]) continue; 
        inSet[u.y][u.x] = 1;
        explorationSteps[explorationStepCount++] = u;

        if (u.x == endPoint.x && u.y == endPoint.y) {
            
            pathLength = 0;
            Point curr = u;
            while (curr.x != -1 && curr.y != -1) {
                path[pathLength++] = curr;
                int px = previous[curr.y][curr.x][0];
                int py = previous[curr.y][curr.x][1];
                curr.x = px;
                curr.y = py;
            }
            break;
        }

        Node* node = graph[u.y][u.x].head;
        while (node != NULL) {
            if (!inSet[node->y][node->x]) {
                int alt = dist[u.y][u.x] + 1; 
                if (alt < dist[node->y][node->x]) {
                    dist[node->y][node->x] = alt;
                    previous[node->y][node->x][0] = u.x;
                    previous[node->y][node->x][1] = u.y;
                    insertHeap(heap, (Point){node->x, node->y}, alt);
                }
            }
            node = node->next;
        }
    }

    freeMinHeap(heap);
}
