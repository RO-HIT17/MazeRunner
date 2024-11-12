# Maze Runner Project

## Overview

The Maze Runner project is a maze-solving application that utilizes various data structures and algorithms to generate and solve mazes. The project is implemented in C and JavaScript, with a WebAssembly bridge to enable interaction between the two languages. This README provides an overview of the data structures used in the project and their roles.

## Data Structures

### 1. MinHeap

The `MinHeap` is a binary heap data structure used to implement Dijkstra's algorithm for finding the shortest path in the maze. It supports the following operations:

- **Create MinHeap**: Initializes a new MinHeap with a given capacity.
- **Insert**: Adds a new element to the heap.
- **Extract Min**: Removes and returns the element with the smallest distance.
- **MinHeapify Up/Down**: Maintains the heap property by adjusting the position of elements.

Relevant code excerpts:
- [Create MinHeap](maze.c#L323)
- [Insert](maze.c#L350)
- [Extract Min](maze.c#L373)
- [MinHeapify Up/Down](maze.c#L323)

### 2. Queue

The `Queue` is a FIFO (First-In-First-Out) data structure used to implement the Breadth-First Search (BFS) algorithm for maze solving. It supports the following operations:

- **Create Queue**: Initializes a new queue with a given capacity.
- **Enqueue**: Adds a new element to the rear of the queue.
- **Dequeue**: Removes and returns the element from the front of the queue.
- **Is Queue Empty**: Checks if the queue is empty.

Relevant code excerpts:
- [Create Queue](maze.c#L286)
- [Enqueue](maze.c#L286)
- [Dequeue](maze.c#L286)
- [Is Queue Empty](maze.c#L286)

### 3. Stack

The `Stack` is a LIFO (Last-In-First-Out) data structure used to implement the Depth-First Search (DFS) algorithm for maze solving. It supports the following operations:

- **Create Stack**: Initializes a new stack with a given capacity.
- **Push**: Adds a new element to the top of the stack.
- **Pop**: Removes and returns the element from the top of the stack.
- **Is Stack Empty**: Checks if the stack is empty.

Relevant code excerpts:
- [Create Stack](maze.c#L241)
- [Push](maze.c#L241)
- [Pop](maze.c#L241)
- [Is Stack Empty](maze.c#L241)

### 4. Graph

The `Graph` is used to represent the maze, where each cell is a node, and edges represent the possible paths between cells. The graph is used in the maze generation and solving algorithms.

Relevant code excerpts:
- [Generate Maze](maze.c#L91)
- [Add Edge](maze.c#L91)

## Algorithms

### 1. Depth-First Search (DFS)

DFS is used to explore the maze by following a path until it reaches the end or a dead end, then backtracking to explore other paths. It uses a stack to keep track of the current path.

Relevant code excerpts:
- [Solve Maze DFS](maze.c#L383)

### 2. Breadth-First Search (BFS)

BFS explores the maze level by level, ensuring the shortest path is found. It uses a queue to keep track of the current level of nodes to explore.

Relevant code excerpts:
- [Solve Maze BFS](maze.c#L445)

### 3. Dijkstra's Algorithm

Dijkstra's algorithm finds the shortest path from the start to the end of the maze using a priority queue (MinHeap) to explore the nodes with the smallest distance first.

Relevant code excerpts:
- [Solve Maze Dijkstra](maze.c#L373)

## WebAssembly Integration

The project uses WebAssembly to bridge the C code with JavaScript, enabling the maze generation and solving algorithms to be executed in the browser.

Relevant code excerpts:
- [WebAssembly Functions](maze.c#L125)

## User Interface

The user interface is implemented in HTML and JavaScript, allowing users to generate and solve mazes using different algorithms. The maze is displayed on an HTML canvas, and user interactions are handled through event listeners.

Relevant code excerpts:
- [HTML Structure](index.html)
- [JavaScript Functions](script.js)

## Conclusion

The Maze Runner project demonstrates the use of various data structures and algorithms to solve a maze. The integration of C and JavaScript through WebAssembly provides a seamless user experience in the browser.