// mazesolver.cpp
#include "mazesolver.h"
#include "utils.h"      // Includes Color namespace
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <cmath>        // For abs()
#include <limits>       // For infinity
#include <chrono>       // For sleep duration
#include <thread>       // For this_thread::sleep_for
#include <iomanip>      // For setw (optional formatting)

// Add this line after includes
using namespace std;

// Adjust delay for visualization speed (milliseconds)
const int VISUALIZATION_DELAY_MS = 50; // Lower value = faster, Higher = slower

// --- Constructor and Loading Logic (logic unchanged, just removed std::) ---
MazeSolver::MazeSolver(const string& filename) {
    if (!loadMaze(filename)) {
        cout << Color::BOLD_RED << "Failed to load maze from '" << filename << "'. Using default maze.\n" << Color::RESET;
        // Define a simple default maze if loading fails
        grid = {
            "S#####",
            ".#...#",
            ".#.#.#",
            ".#...#",
            "####E#"
        };
        rows = grid.size();
        if (rows > 0) cols = grid[0].size();
        // Find S and E in default maze
        startPoint = Point{-1, -1};
        endPoint = Point{-1, -1};
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] == START) startPoint = Point{r, c};
                else if (grid[r][c] == END) endPoint = Point{r, c};
            }
        }
         if (startPoint.r == -1 || endPoint.r == -1) {
              cerr << Color::BOLD_RED << "Error: Default maze is missing Start ('S') or End ('E').\n" << Color::RESET;
              grid.clear();
              rows = 0; cols = 0;
         }
    }
}

bool MazeSolver::loadMaze(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << Color::BOLD_RED << "Error: Cannot open maze file '" << filename << "'.\n" << Color::RESET;
        return false;
    }

    grid.clear();
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            grid.push_back(line);
        }
    }
    file.close();

    if (grid.empty()) {
         cerr << Color::BOLD_RED << "Error: Maze file '" << filename << "' is empty or contains only empty lines.\n" << Color::RESET;
         return false;
    }

    rows = grid.size();
    cols = grid[0].size();
    if (cols == 0) { // Check if rows are empty strings
        cerr << Color::BOLD_RED << "Error: Maze file contains empty rows.\n" << Color::RESET;
        grid.clear(); return false;
    }

    startPoint = Point{-1, -1};
    endPoint = Point{-1, -1};
    for (int r = 0; r < rows; ++r) {
        if (grid[r].length() != static_cast<size_t>(cols)) {
             cerr << Color::BOLD_RED << "Error: Maze rows have inconsistent lengths (Row " << r << " has "
                  << grid[r].length() << ", expected " << cols << ").\n" << Color::RESET;
             grid.clear(); return false;
        }
        for (int c = 0; c < cols; ++c) {
            char cell = grid[r][c];
            if (cell == START) {
                if (startPoint.r != -1) { cerr << Color::YELLOW << "Warning: Multiple start points ('S') found. Using the last one at (" << r << "," << c << ").\n" << Color::RESET; }
                startPoint = Point{r, c};
            } else if (cell == END) {
                 if (endPoint.r != -1) { cerr << Color::YELLOW << "Warning: Multiple end points ('E') found. Using the last one at (" << r << "," << c << ").\n" << Color::RESET; }
                endPoint = Point{r, c};
            } else if (cell != WALL && cell != PATH) {
                 cerr << Color::YELLOW << "Warning: Unexpected character '" << cell << "' at (" << r << "," << c
                      << "). Treating as PATH ('.').\n" << Color::RESET;
                 grid[r][c] = PATH; // Standardize unexpected characters to PATH
            }
        }
    }

    if (startPoint.r == -1) {
         cerr << Color::BOLD_RED << "Error: Start point ('S') not found in maze '" << filename << "'.\n" << Color::RESET;
         grid.clear(); return false;
    }
     if (endPoint.r == -1) {
         cerr << Color::BOLD_RED << "Error: End point ('E') not found in maze '" << filename << "'.\n" << Color::RESET;
         grid.clear(); return false;
     }

    return true;
}
// --- End Loading Logic ---


// --- Modified displayMaze with Enhanced UI ---
void MazeSolver::displayMaze(bool showVisited) const {
    cout << "\n" << Color::WHITE << "Maze (" << rows << "x" << cols << "):" << Color::RESET << "\n";
    // Top border
    cout << Color::WHITE << " +" << string(cols, '-') << "+" << Color::RESET << "\n";

    for (int r = 0; r < rows; ++r) {
        cout << Color::WHITE << " |" << Color::RESET; // Left border
        for (int c = 0; c < cols; ++c) {
            // Check bounds before accessing grid element
            if (r >= 0 && r < grid.size() && c >= 0 && c < grid[r].size()) {
                char cell = grid[r][c];
                // Choose color based on cell type
                switch(cell) {
                    case WALL:          cout << Color::WHITE << '#' << Color::RESET; break; // White Wall
                    case START:         cout << Color::BOLD_GREEN << 'S' << Color::RESET; break; // Green Start
                    case END:           cout << Color::BOLD_RED << 'E' << Color::RESET; break;   // Red End
                    case SOLUTION_PATH: cout << Color::BOLD_YELLOW << '*' << Color::RESET; break; // Yellow Solution Path
                    case VISITED:
                        if (showVisited) cout << Color::CYAN << '+' << Color::RESET; // Cyan Visited node
                        else cout << Color::WHITE << '.' << Color::RESET;             // Show as path if not visualizing visited
                        break;
                    case PATH:
                    default:            cout << Color::WHITE << '.' << Color::RESET; break; // White Path
                }
            } else {
                cout << '?'; // Should not happen if loadMaze validates correctly
            }
        }
        cout << Color::WHITE << "|" << Color::RESET << "\n"; // Right border and newline
    }
    // Bottom border
    cout << Color::WHITE << " +" << string(cols, '-') << "+" << Color::RESET << "\n\n";

    // Display Legend
    cout << " Legend: "
         << Color::BOLD_GREEN << "S" << Color::RESET << "=Start  "
         << Color::BOLD_RED << "E" << Color::RESET << "=End  "
         << Color::WHITE << "#" << Color::RESET << "=Wall  "
         << Color::WHITE << "." << Color::RESET << "=Path  "
         << Color::BOLD_YELLOW << "*" << Color::RESET << "=Solution Path"
         << (showVisited ? "  " + Color::CYAN + "+" + Color::RESET + "=Visited" : "") // Only show visited legend if applicable
         << "\n\n";
}
// --- End displayMaze ---


// --- Game Logic (isValid, calculateHeuristic, reconstructPath) - logic unchanged ---
bool MazeSolver::isValid(int r, int c) const {
    return r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] != WALL;
}

int MazeSolver::calculateHeuristic(Point a, Point b) const {
    return abs(a.r - b.r) + abs(a.c - b.c);
}

void MazeSolver::reconstructPath(const map<int, Point>& cameFrom, Point current) {
     Point temp = current;
     int currentIdx = pointToIndex(temp);
     while (cameFrom.count(currentIdx)) {
         temp = cameFrom.at(currentIdx);
         currentIdx = pointToIndex(temp);
         if (temp.r == startPoint.r && temp.c == startPoint.c) break; // Stop if we backtrack to start

         // Only mark PATH cells as SOLUTION_PATH
         if (grid[temp.r][temp.c] == PATH || grid[temp.r][temp.c] == VISITED) {
             grid[temp.r][temp.c] = SOLUTION_PATH;
         }
         // Safety check (optional, A* shouldn't cycle)
         // if (currentIdx == pointToIndex(cameFrom.at(currentIdx))) break;
     }
}
// --- End Game Logic ---


// --- Modified solveAStar with Enhanced Visualization Output ---
bool MazeSolver::solveAStar() {
    // Use a copy for visualization steps
    vector<string> displayGrid = grid;

    // Priority queue (min-heap based on fCost)
    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    // Map to track path predecessors
    map<int, Point> cameFrom;
    // Map to store the cheapest cost found so far to reach a node
    map<int, int> gCost;

    // Initialize gCost for all nodes to infinity
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            gCost[pointToIndex(Point{r,c})] = numeric_limits<int>::max();
        }
    }

    // Check start point validity before using it
    if (startPoint.r < 0 || startPoint.r >= rows || startPoint.c < 0 || startPoint.c >= cols) {
         cerr << Color::BOLD_RED << "Error: Invalid start point coordinates for A*.\n" << Color::RESET;
         return false;
    }

    // Initialize start node
    gCost[pointToIndex(startPoint)] = 0;
    Node startNode;
    startNode.pos = startPoint;
    startNode.gCost = 0;
    startNode.hCost = calculateHeuristic(startPoint, endPoint);
    startNode.parent = Point{-1, -1}; // No parent for start
    openSet.push(startNode);

    // Main A* loop
    while (!openSet.empty()) {
        Node current = openSet.top(); // Get node with lowest fCost
        openSet.pop();
        int currentIdx = pointToIndex(current.pos);

        // Optimization: If this node was already processed via a shorter path, skip.
        // (Requires tracking closed set or comparing current.gCost > gCost[currentIdx])
        // For simplicity, we rely on the gCost check below.


        // --- Visualization Step ---
        // Mark current node as visited on the temporary display grid
        if (displayGrid[current.pos.r][current.pos.c] != START && displayGrid[current.pos.r][current.pos.c] != END) {
             displayGrid[current.pos.r][current.pos.c] = VISITED;
        }
        clearScreen();
        cout << Color::BOLD_YELLOW << "=== Maze Solver (A*) - Searching... ===" << Color::RESET << "\n";

        // Display the current state using the enhanced displayMaze
        // Temporarily assign displayGrid to the object's grid for the function call
        vector<string> backupGrid = this->grid;
        this->grid = displayGrid;
        displayMaze(true); // Show visited nodes during search
        this->grid = backupGrid; // Restore the actual grid

        // Print status message
        cout << " Exploring: (" << Color::CYAN << current.pos.r << Color::RESET << ","
             << Color::CYAN << current.pos.c << Color::RESET << ") "
             << " fCost=" << Color::YELLOW << current.fCost() << Color::RESET
             << " (g=" << current.gCost << ", h=" << current.hCost << ")" << "\n";
        cout.flush(); // Ensure output is visible before sleep
        this_thread::sleep_for(chrono::milliseconds(VISUALIZATION_DELAY_MS));
        // --- End Visualization Step ---


        // Goal check
        if (current.pos.r == endPoint.r && current.pos.c == endPoint.c) {
            reconstructPath(cameFrom, current.pos); // Modify the member 'grid' with the solution path
            return true; // Path found
        }

        // Explore neighbors (Up, Down, Left, Right)
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int nr = current.pos.r + dr[i];
            int nc = current.pos.c + dc[i];
            Point neighborPos = Point{nr, nc};

            // Use the member 'grid' for validity checks (walls don't change)
            if (isValid(nr, nc)) {
                int neighborIdx = pointToIndex(neighborPos);
                int tentative_gCost = gCost[currentIdx] + 1; // Cost to move to neighbor is 1

                // If this path to the neighbor is better than any previous one found
                if (tentative_gCost < gCost[neighborIdx]) {
                    // Update path information
                    cameFrom[neighborIdx] = current.pos;
                    gCost[neighborIdx] = tentative_gCost;

                    // Create neighbor node and add to open set
                    Node neighborNode;
                    neighborNode.pos = neighborPos;
                    neighborNode.gCost = tentative_gCost;
                    neighborNode.hCost = calculateHeuristic(neighborPos, endPoint);
                    // neighborNode.parent = current.pos; // Parent info is in cameFrom map

                    openSet.push(neighborNode);
                }
            }
        }
    } // End while loop

    return false; // No path found (openSet is empty)
}
// --- End solveAStar ---


// --- Modified play() function with Enhanced UI ---
void MazeSolver::play() {
    string mazeFilename = "maze.txt"; // Default filename

    // Load/reload maze at the start of play
    if (!loadMaze(mazeFilename)) {
        cout << Color::BOLD_RED << "Error loading maze '" << mazeFilename << "'. Cannot start game.\n" << Color::RESET;
        // No pressEnterToContinue here, let main loop handle it after return
        return;
    }

    // Check if maze loaded correctly (start/end points found)
    if (grid.empty() || startPoint.r == -1 || endPoint.r == -1) {
         cout << Color::BOLD_RED << "Maze seems invalid after loading. Cannot solve.\n" << Color::RESET;
         return;
    }

    // Keep a pristine copy of the loaded grid
    vector<string> originalGrid = grid;

    clearScreen();
    cout << Color::BOLD_YELLOW << "=== Maze Solver (A*) ===\n" << Color::RESET;
    cout << "Initial Maze:\n";
    displayMaze(); // Display initial maze with colors

    cout << Color::CYAN << "Press Enter to start solving..." << Color::RESET;
    cout.flush();
    // Clear buffer before waiting for Enter
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if(cin.peek() == '\n') cin.ignore(); // Handle potential immediate newline
    cin.get();

    // Run the A* algorithm (handles its own visualization)
    // It modifies the member 'grid' if a path is found
    bool pathFound = solveAStar();

    // Display Final Result
    clearScreen(); // Clear the last frame of the visualization
    if (pathFound) {
        cout << Color::BOLD_GREEN << "=== Maze Solver (A*) - Path Found! ===\n" << Color::RESET;
        cout << "Solved Maze:\n";
        displayMaze(); // Display the member 'grid' which now contains '*' path
    } else {
        cout << Color::BOLD_RED << "=== Maze Solver (A*) - No Path Found ===\n" << Color::RESET;
        cout << "No path found from Start ('S') to End ('E').\n";
        cout << "Maze State After Search:\n";
        // Restore original grid before displaying 'no path' result for clarity
        grid = originalGrid;
        displayMaze(false); // Display original maze structure without visited marks
    }

    // Restore the object's grid state to the original loaded version
    // This ensures subsequent plays (if the object persists) start fresh.
    grid = originalGrid;

    // Pause is handled by main.cpp after this function returns
}
// --- End play() function ---