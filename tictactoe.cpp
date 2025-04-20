#include "tictactoe.h"
#include "utils.h"      // For clearScreen, pressEnterToContinue, getIntInput, Color namespace
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>    // For max and min
#include <iomanip>      // For setw (formatting)
#include <thread>       // For this_thread::sleep_for
#include <chrono>       // For chrono::milliseconds

// Add this line after includes
using namespace std;

// --- Definitions for static const members ---
// These provide the actual storage for the static const members declared in the header.
const char TicTacToe::HUMAN_PLAYER;
const char TicTacToe::AI_PLAYER;
const char TicTacToe::EMPTY_SLOT;
const int TicTacToe::BOARD_SIZE;
// --- End definitions ---

TicTacToe::TicTacToe() {
    // Constructor remains simple, board initialized in play()
}

void TicTacToe::initializeBoard() {
    // Use vector's assign method for clean initialization
    board.assign(BOARD_SIZE, vector<char>(BOARD_SIZE, EMPTY_SLOT));
}

// --- Modified displayBoard with UI enhancements ---
void TicTacToe::displayBoard() const {
    cout << "\n";
    // Print column numbers centered above the board
    cout << "   "; // Initial padding to align with board lines
    for (int j = 0; j < BOARD_SIZE; ++j) {
        // setw(2) ensures consistent spacing for numbers
        cout << Color::WHITE << setw(2) << j << Color::RESET << " ";
    }
    cout << "\n";

    // Top border of the board using box-drawing characters
    cout << "  " << Color::WHITE << "╔"; // Top-left corner
    for (int j = 0; j < BOARD_SIZE; ++j) {
        cout << "═══"; // Horizontal line segment for a cell
        cout << (j < BOARD_SIZE - 1 ? "╦" : "╗"); // T-connector or Top-right corner
    }
    cout << Color::RESET << "\n";

    // Iterate through each row of the board
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Print row number at the start of the line
        cout << Color::WHITE << setw(2) << i << Color::RESET << " ";
        cout << Color::WHITE << "║" << Color::RESET; // Left vertical border

        // Iterate through columns in the current row
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << " "; // Padding inside the cell
            char player = board[i][j]; // Get the character in the current cell

            // Display player markers with color
            if (player == HUMAN_PLAYER) {
                cout << Color::BOLD_RED << player << Color::RESET; // Human ('X') in Red
            } else if (player == AI_PLAYER) {
                cout << Color::BOLD_BLUE << player << Color::RESET; // AI ('O') in Blue
            } else {
                cout << EMPTY_SLOT; // Display empty slot character
            }
            cout << " " << Color::WHITE << "║" << Color::RESET; // Right vertical separator for the cell
        }
        cout << "\n"; // End of the row

        // Print separator line between rows (unless it's the last row)
        if (i < BOARD_SIZE - 1) {
            cout << "  " << Color::WHITE << "╠"; // Left T-connector
            for (int j = 0; j < BOARD_SIZE; ++j) {
                cout << "═══"; // Horizontal line segment
                cout << (j < BOARD_SIZE - 1 ? "╬" : "╣"); // Cross-connector or Right T-connector
            }
             cout << Color::RESET << "\n";
        }
    }

    // Bottom border of the board
    cout << "  " << Color::WHITE << "╚"; // Bottom-left corner
    for (int j = 0; j < BOARD_SIZE; ++j) {
        cout << "═══"; // Horizontal line segment
        cout << (j < BOARD_SIZE - 1 ? "╩" : "╝"); // Bottom T-connector or Bottom-right corner
    }
    cout << Color::RESET << "\n\n"; // Add extra newlines for spacing
}
// --- End displayBoard ---


// --- isValidMove, checkWin, isBoardFull, checkGameOver (logic unchanged) ---
bool TicTacToe::isValidMove(int row, int col) const {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == EMPTY_SLOT;
}

bool TicTacToe::checkWin(char player) const {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
    }
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
    return false;
}

bool TicTacToe::isBoardFull() const {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY_SLOT) {
                return false;
            }
        }
    }
    return true;
}

bool TicTacToe::checkGameOver(char& winner) {
     if (checkWin(HUMAN_PLAYER)) {
        winner = HUMAN_PLAYER;
        return true;
    }
    if (checkWin(AI_PLAYER)) {
        winner = AI_PLAYER;
        return true;
    }
    if (isBoardFull()) {
        winner = EMPTY_SLOT; // Indicate draw
        return true;
    }
    winner = ' '; // Game not over
    return false;
}
// --- End logic functions ---


// --- Modified getPlayerMove with colored prompt and error messages ---
void TicTacToe::getPlayerMove(int& row, int& col) {
     while (true) {
        // Use colors for player prompt
        cout << "Player " << Color::BOLD_RED << HUMAN_PLAYER << Color::RESET
                  << ", enter move (" << Color::CYAN << "row col" << Color::RESET << "): ";
        cout.flush(); // Ensure prompt is displayed before waiting for input

        // Check if input extraction fails (e.g., non-numeric input)
        if (!(cin >> row >> col)) {
             cout << Color::BOLD_RED << "Invalid input format. Please enter two numbers separated by a space.\n" << Color::RESET;
             cin.clear(); // Clear error flags on cin
             // Discard the rest of the invalid input line
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             continue; // Ask for input again
        }
        // Discard any extra characters entered on the line after the numbers
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // Check if the move is valid according to game rules
        if (isValidMove(row, col)) {
            break; // Valid move entered, exit loop
        } else {
            // Provide specific feedback if the move is invalid
            cout << Color::BOLD_RED << "Invalid move. ";
            if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
                 cout << "Row and column must be between 0 and " << BOARD_SIZE - 1 << ".\n";
            } else {
                 cout << "Cell (" << row << ", " << col << ") is already taken.\n";
            }
             cout << Color::RESET;
        }
    }
}
// --- End getPlayerMove ---


// --- Minimax AI Implementation (logic unchanged) ---
TicTacToe::Move TicTacToe::findBestMove() {
    int bestScore = numeric_limits<int>::min();
    Move bestMove;
    bestMove.row = -1;
    bestMove.col = -1;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == EMPTY_SLOT) {
                board[i][j] = AI_PLAYER;
                int moveScore = minimax(0, false);
                board[i][j] = EMPTY_SLOT; // Undo move

                if (moveScore > bestScore) {
                    bestScore = moveScore;
                    bestMove.row = i;
                    bestMove.col = j;
                }
            }
        }
    }
    bestMove.score = bestScore;
    return bestMove;
}

int TicTacToe::minimax(int depth, bool isMaximizingPlayer) {
    char winner;
    if (checkGameOver(winner)) {
        if (winner == AI_PLAYER) return 10 - depth;
        if (winner == HUMAN_PLAYER) return -10 + depth;
        return 0; // Draw
    }

    if (isMaximizingPlayer) { // AI's turn (O)
        int bestScore = numeric_limits<int>::min();
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == EMPTY_SLOT) {
                    board[i][j] = AI_PLAYER;
                    bestScore = max(bestScore, minimax(depth + 1, false));
                    board[i][j] = EMPTY_SLOT;
                }
            }
        }
        return bestScore;
    } else { // Human's turn (X)
        int bestScore = numeric_limits<int>::max();
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == EMPTY_SLOT) {
                    board[i][j] = HUMAN_PLAYER;
                    bestScore = min(bestScore, minimax(depth + 1, true));
                    board[i][j] = EMPTY_SLOT;
                }
            }
        }
        return bestScore;
    }
}
// --- End AI Implementation ---


// --- Modified Main Play Loop with UI enhancements ---
void TicTacToe::play() {
    initializeBoard(); // Set up the board
    char currentPlayer = HUMAN_PLAYER; // Human (X) starts
    char winner = ' ';
    bool gameOver = false;

    // Main game loop continues until game is over
    while (!gameOver) {
        clearScreen(); // Clear screen at the start of each turn
        // Display colored title
        cout << Color::BOLD_YELLOW << "=== Tic-Tac-Toe ===\n" << Color::RESET;
        displayBoard(); // Display the current board state

        string status; // To display whose turn it is
        if (currentPlayer == HUMAN_PLAYER) {
            // Display human player's turn message
            status = "Your turn (Player " + Color::BOLD_RED + HUMAN_PLAYER + Color::RESET + ")";
            cout << status << "\n";
            int r, c;
            getPlayerMove(r, c); // Get validated move from human
            board[r][c] = HUMAN_PLAYER; // Place human's piece on the board
        } else { // AI_PLAYER's turn
            // Display AI player's turn message
            status = "AI's turn (" + Color::BOLD_BLUE + AI_PLAYER + Color::RESET + ")... Thinking...";
            cout << status << "\n";
            cout.flush(); // Ensure message is shown before delay

            // Add a small delay to simulate AI thinking
            this_thread::sleep_for(chrono::milliseconds(500)); // 0.5 second delay

            Move aiMove = findBestMove(); // Calculate the AI's best move using Minimax

            // Place AI's piece if a valid move was found
            if (aiMove.row != -1) {
                 board[aiMove.row][aiMove.col] = AI_PLAYER;
                 // Optional: Add a message indicating AI's move choice
                 // cout << "AI chose: (" << aiMove.row << ", " << aiMove.col << ")" << endl;
                 // this_thread::sleep_for(chrono::milliseconds(750)); // Short pause to see AI move
            } else {
                 // This should ideally not happen in TicTacToe if game isn't over
                cout << Color::BOLD_RED << "AI error: Could not find a valid move.\n" << Color::RESET;
                this_thread::sleep_for(chrono::seconds(1)); // Pause to see error
                gameOver = true; // Force end game if AI fails unexpectedly
            }
        }

        // Check if the game has ended after the current move
        gameOver = checkGameOver(winner);

        // If the game is not over, switch the current player
        if (!gameOver) {
            currentPlayer = (currentPlayer == HUMAN_PLAYER) ? AI_PLAYER : HUMAN_PLAYER;
        }
    } // End of game loop

    // --- Game Over Section ---
    clearScreen(); // Clear screen for the final result display
    // Display colored game over title
    cout << Color::BOLD_YELLOW << "=== Tic-Tac-Toe: Game Over ===\n" << Color::RESET;
    displayBoard(); // Show the final board state

    // Display the outcome message with color
    if (winner == HUMAN_PLAYER) {
        cout << Color::BOLD_GREEN << "*** Congratulations! You (" << HUMAN_PLAYER << ") win! ***\n" << Color::RESET;
    } else if (winner == AI_PLAYER) {
        cout << Color::BOLD_RED << "*** AI Player (" << AI_PLAYER << ") wins! ***\n" << Color::RESET;
    } else { // Draw condition
        cout << Color::BOLD_YELLOW << "*** It's a Draw! ***\n" << Color::RESET;
    }
    // --- End Game Over Section ---

    // The pause (`pressEnterToContinue()`) is handled by the main loop in main.cpp
}
// --- End play() function ---