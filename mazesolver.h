#ifndef MAZESOLVER_H
#define MAZESOLVER_H

#include "game.h"
#include <vector>
#include <string>
#include <queue> // For priority_queue
#include <map>   // For tracking costs and parents

class MazeSolver : public Game {
public:
    // Constructor takes filename or uses a default maze
    MazeSolver(const std::string& filename = "maze.txt");
    void play() override;
    std::string getName() const override { return "Maze Solver (A*)"; }
    virtual ~MazeSolver() = default;

private:
    // Maze representation & constants
    std::vector<std::string> grid;
    int rows = 0;
    int cols = 0;
    static const char WALL = '#';
    static const char PATH = '.';
    static const char START = 'S';
    static const char END = 'E';
    static const char SOLUTION_PATH = '*';
    static const char VISITED = '+'; // Mark visited during search

    // Start and end points
    struct Point { int r = -1, c = -1; };
    Point startPoint, endPoint;

    // A* specific data structures
    struct Node {
        Point pos;
        int gCost = 0; // Cost from start
        int hCost = 0; // Heuristic cost to end
        int fCost() const { return gCost + hCost; } // Total estimated cost
        Point parent;

        // For priority queue ordering (min-heap based on fCost)
        bool operator>(const Node& other) const {
            return fCost() > other.fCost();
        }
         bool operator==(const Point& p) const { // Compare Node with Point
            return pos.r == p.r && pos.c == p.c;
        }
    };

    // Helper methods
    bool loadMaze(const std::string& filename);
    void displayMaze(bool showVisited = false) const; // Option to show search path
    bool isValid(int r, int c) const;
    int calculateHeuristic(Point a, Point b) const; // Manhattan distance
    bool solveAStar(); // Main A* algorithm
    void reconstructPath(const std::map<int, Point>& cameFrom, Point current);
    int pointToIndex(Point p) const { return p.r * cols + p.c; } // Helper to use Point as map key
   // Old:
// Point indexToPoint(int index) const { return {index / cols, index % cols}; } // Helper
// New:
Point indexToPoint(int index) const { return Point{index / cols, index % cols}; } // Helper
};

#endif // MAZESOLVER_H