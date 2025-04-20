#ifndef CONNECTFOUR_H
#define CONNECTFOUR_H

#include "game.h"
#include <vector>
#include <string>

class ConnectFour : public Game {
public:
    ConnectFour();
    void play() override;
    std::string getName() const override { return "Connect Four"; }
    virtual ~ConnectFour() = default;

private:
    // Constants
    static const int ROWS = 6;
    static const int COLS = 7;
    static const char HUMAN_PLAYER = 'X';
    static const char AI_PLAYER = 'O';
    static const char EMPTY_SLOT = '.';
    static const int WIN_LENGTH = 4;
    static const int MAX_DEPTH = 5; // Adjust AI difficulty (search depth)

    // Board
    std::vector<std::vector<char>> board;

    // Game Logic
    void initializeBoard();
    void displayBoard() const;
    bool isValidColumn(int col) const; // Check if column has space
    bool dropPiece(int col, char player); // Returns true if successful
    int getNextOpenRow(int col) const;
    bool checkWin(char player) const;
    bool isBoardFull() const;
    bool checkGameOver(char& winner);
    int getPlayerMove(); // Returns column choice

    // AI - Minimax with Alpha-Beta and Heuristics
    struct Move {
        int col = -1;
        int score = 0;
    };

    Move findBestMove();
    int minimaxAlphaBeta(int depth, int alpha, int beta, bool isMaximizingPlayer);
    int evaluateBoard() const; // Heuristic function
    int scoreLine(const std::vector<char>& line, char player) const; // Helper for evaluation
};

#endif // CONNECTFOUR_H