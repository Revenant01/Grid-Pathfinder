# Simple GPS

A console-based GPS pathfinding simulator written in C. It models a 10×10 grid map, lets the user choose a travel mode (walking or driving), enter departure and destination coordinates, and then finds all valid routes using depth-first search — reporting the shortest path with step-by-step directions.

## How It Works

The map is a 10×10 grid where each cell holds either `0` (walkable/footpath) or `1` (drivable/road). Cells are randomly assigned at startup.

The user picks a **travel mode**:
- `0` — Walking (can only move through cells with value `0`)
- `1` — Driving (can only move through cells with value `1`)

Departure and destination coordinates must both be cells of the chosen type. The pathfinder then performs a recursive DFS across the grid, collecting every valid route. After the search, the shortest route is printed as a sequence of coordinates and cardinal directions.

## Project Structure

```
simple-GPS/
├── finalGPS.c      # Full source code
├── STD_TYPES.h     # Custom type definitions (u8, u32, etc.)
└── README.md       # This file
```

## Data Structures

### `Pair`
Holds a grid coordinate as a row/column pair (`x`, `y`).

### `Node`
A singly-linked list node that wraps a `Pair` and a `next` pointer.

### `LinkedList`
A linked list with both `head` and `last` pointers for O(1) tail insertion. Each instance represents one complete route from departure to destination.

### `ans[]`
A global array of `LinkedList*` (capacity `SIZE = 500`) that accumulates every valid route found during the DFS.

## Key Functions

| Function | Description |
|---|---|
| `fill_Random()` | Fills the map grid with random `0`/`1` values. |
| `printBoard()` | Renders the grid to the console with row/column labels. |
| `options()` | Prompts for travel mode; loops until `0` or `1` is entered. |
| `departure_location()` | Reads the starting row and column from the user. |
| `arrival_location()` | Reads the destination row and column from the user. |
| `helper()` | Bootstraps the DFS: creates the initial node and linked list. |
| `graph()` | Recursive DFS. Explores up/down/left/right neighbors that match the travel mode and haven't been visited. When destination is reached, clones the current path into `ans[]`. |
| `printOptions()` | Iterates `ans[]`, finds the shortest route by node count, prints its coordinates and directions. |
| `printDirections()` | Walks a route's linked list and emits `UP`, `DOWN`, `LEFT`, or `RIGHT` for each step. |
| `createNode()` | Allocates and initializes a `Node` on the heap. |
| `creatLinkedList()` | Allocates and initializes an empty `LinkedList` on the heap. |
| `insertLast()` | Appends a node to the tail of a linked list. |
| `clone()` | Deep-copies a linked list (used to branch the current path during DFS). |
| `insertInArray()` | Saves a completed route into `ans[]` and increments `optionCounter`. |
| `isEmpty()` | Returns `1` if the linked list has no nodes, `0` otherwise. |
| `linkedlistSize()` / `sizeofLinkedList()` | Return the number of nodes in a linked list. |

## Building

The project depends on `STD_TYPES.h` for fixed-width type aliases (`u8`, `u32`). Provide that header alongside the source file, then compile with any C99-compatible compiler:

```bash
gcc finalGPS.c -o gps
```

> **Note:** The code calls `system("cls")` (Windows `cmd` command) to clear the screen before printing the final board. On Linux/macOS replace it with `system("clear")`, or remove the call entirely.

## Running

```
./gps
```

**Example session:**

```
    0 1 2 3 4 5 6 7 8 9
   --------------------
0 | 1 0 1 0 1 1 0 1 0 1
...

0-walking.
1-driving.

please choose an option: 1
You are using the driving mode

please enter your current position
please enter the row number: 0
please enter the coloumn number: 0

please enter your destination
please enter the row number: 3
please enter the coloumn number: 5

The shortest route
(0,0) (1,0) (2,0) (2,1) (3,1) (3,2) (3,3) (3,4) (3,5)
START-> DOWN-> DOWN-> RIGHT-> DOWN-> RIGHT-> RIGHT-> RIGHT-> RIGHT->  END
```

## Algorithm

The pathfinder uses **recursive DFS with backtracking**:

1. Mark the current cell as visited.
2. Try each of the four neighbors (up, down, right, left) that:
   - Are within grid bounds,
   - Match the selected travel mode value, and
   - Have not yet been visited on this path.
3. Clone the current route linked list, append the neighbor, and recurse.
4. On return, unmark the current cell (backtrack) so other branches can use it.
5. When the current cell equals the destination, save a clone of the route to `ans[]`.

After the full search, `printOptions()` scans `ans[]` for the route with the fewest nodes and prints it.

## Limitations

- The `visited[][]` array is global and shared across all recursive branches, which is standard for DFS backtracking but means the search is not thread-safe.
- Memory allocated for routes and nodes is not freed — for a short-lived CLI program this is acceptable, but a production version should add cleanup.
- The maximum number of stored routes is hard-coded at `SIZE = 500`; deeply connected grids could theoretically exceed this.
- `system("cls")` is Windows-specific.
