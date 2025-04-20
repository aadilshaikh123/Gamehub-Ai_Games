#ifndef TICTACTOE_H
#define TICTACTOE_H

#include "game.h"
#include <vector>
#include <string>

class TicTacToe : public Game {
public:
    TicTacToe();
    void play() override; // Implement the pure virtual function
    std::string getName() const override { return "Tic-Tac-Toe"; }
    virtual ~TicTacToe() = default; // Use default destructor

private:
    // Constants
    static const char HUMAN_PLAYER = 'X';
    static const char AI_PLAYER = 'O';
    static const char EMPTY_SLOT = ' ';
    static const int BOARD_SIZE = 3;

    // Board representation
    std::vector<std::vector<char>> board;

    // Internal game logic methods
    void initializeBoard();
    void displayBoard() const;
    bool isValidMove(int row, int col) const;
    bool checkWin(char player) const;
    bool isBoardFull() const;
    bool checkGameOver(char& winner); // Returns true if game over, sets winner
    void getPlayerMove(int& row, int& col);

    // AI specific methods
    struct Move {
        int row = -1, col = -1;
        int score = 0; // Score associated with this move
    };

    Move findBestMove();
    int minimax(int depth, bool isMaximizingPlayer); // Pass board implicitly as member
};

#endif // TICTACTOE_H