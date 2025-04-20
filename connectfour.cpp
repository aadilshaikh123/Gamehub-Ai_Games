#include "connectfour.h"
#include "utils.h"      // Includes Color namespace
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>    // For max, min
#include <iomanip>      // For setw
#include <string>       // For string manipulation in getPlayerMove
#include <thread>       // For this_thread::sleep_for
#include <chrono>       // For chrono::milliseconds

// Add this line after includes
using namespace std;

// --- Definitions for static const members ---
const int ConnectFour::ROWS;
const int ConnectFour::COLS;
const char ConnectFour::HUMAN_PLAYER;
const char ConnectFour::AI_PLAYER;
const char ConnectFour::EMPTY_SLOT;
const int ConnectFour::WIN_LENGTH;
const int ConnectFour::MAX_DEPTH;
// --- End definitions ---

ConnectFour::ConnectFour() {
    // Constructor - board initialized in play()
}

void ConnectFour::initializeBoard() {
    board.assign(ROWS, vector<char>(COLS, EMPTY_SLOT));
}

// --- Modified displayBoard with colors and better structure ---
void ConnectFour::displayBoard() const {
    cout << "\n";
    // Print column numbers centered above each column
    cout << " "; // Initial padding
    for (int j = 0; j < COLS; ++j) {
        // setw(3) provides space: one for left padding, one for number, one for right padding
        cout << Color::WHITE << setw(3) << j << Color::RESET;
    }
    cout << "\n";

    // Print top border using box-drawing characters
    cout << Color::WHITE << "╔"; // Top-left corner
    for (int j = 0; j < COLS; ++j) {
        cout << "═══" << (j < COLS - 1 ? "╦" : "╗"); // Horizontal line and T-connector or Top-right
    }
    cout << Color::RESET << "\n";

    // Print board content row by row
    for (int i = 0; i < ROWS; ++i) {
        cout << Color::WHITE << "║" << Color::RESET; // Left vertical border for the row
        for (int j = 0; j < COLS; ++j) {
            cout << " "; // Padding inside the cell
            char player = board[i][j];
            // Display player pieces with distinct colors
            if (player == HUMAN_PLAYER) {
                cout << Color::BOLD_RED << player << Color::RESET;    // Human ('X') in Red
            } else if (player == AI_PLAYER) {
                cout << Color::BOLD_YELLOW << player << Color::RESET; // AI ('O') in Yellow
            } else {
                cout << Color::WHITE << EMPTY_SLOT << Color::RESET;   // Empty slot marker (e.g., '.')
            }
            cout << " " << Color::WHITE << "║" << Color::RESET; // Right vertical separator for the cell
        }
        cout << "\n"; // End of the row

        // Print separator line between rows (unless it's the last row)
        if (i < ROWS - 1) {
            cout << Color::WHITE << "╠"; // Left T-connector
            for (int j = 0; j < COLS; ++j) {
                cout << "═══" << (j < COLS - 1 ? "╬" : "╣"); // Cross-connector or Right T-connector
            }
             cout << Color::RESET << "\n";
        }
    }

    // Print bottom border
    cout << Color::WHITE << "╚"; // Bottom-left corner
    for (int j = 0; j < COLS; ++j) {
        cout << "═══" << (j < COLS - 1 ? "╩" : "╝"); // Bottom T-connector or Bottom-right corner
    }
    cout << Color::RESET << "\n\n"; // Add extra spacing after the board
}
// --- End displayBoard ---


// --- Game Logic (isValidColumn, getNextOpenRow, dropPiece, checkWin, isBoardFull, checkGameOver) - unchanged ---
bool ConnectFour::isValidColumn(int col) const {
    return col >= 0 && col < COLS && board[0][col] == EMPTY_SLOT;
}

int ConnectFour::getNextOpenRow(int col) const {
    for (int r = ROWS - 1; r >= 0; --r) {
        if (board[r][col] == EMPTY_SLOT) {
            return r;
        }
    }
    return -1; // Column is full
}

bool ConnectFour::dropPiece(int col, char player) {
    int row = getNextOpenRow(col);
    if (row != -1) {
        board[row][col] = player;
        return true;
    }
    return false; // Column was full
}

bool ConnectFour::checkWin(char player) const {
    // Horizontal
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - WIN_LENGTH; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_LENGTH; ++k) if (board[r][c + k] != player) { win = false; break; }
            if (win) return true;
        }
    }
    // Vertical
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS - WIN_LENGTH; ++r) {
            bool win = true;
            for (int k = 0; k < WIN_LENGTH; ++k) if (board[r + k][c] != player) { win = false; break; }
            if (win) return true;
        }
    }
    // Positive Diagonal
    for (int r = 0; r <= ROWS - WIN_LENGTH; ++r) {
        for (int c = 0; c <= COLS - WIN_LENGTH; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_LENGTH; ++k) if (board[r + k][c + k] != player) { win = false; break; }
            if (win) return true;
        }
    }
    // Negative Diagonal
    for (int r = WIN_LENGTH - 1; r < ROWS; ++r) {
        for (int c = 0; c <= COLS - WIN_LENGTH; ++c) {
            bool win = true;
            for (int k = 0; k < WIN_LENGTH; ++k) if (board[r - k][c + k] != player) { win = false; break; }
            if (win) return true;
        }
    }
    return false;
}

bool ConnectFour::isBoardFull() const {
    for (int c = 0; c < COLS; ++c) {
        if (board[0][c] == EMPTY_SLOT) return false;
    }
    return true;
}

bool ConnectFour::checkGameOver(char& winner) {
    if (checkWin(HUMAN_PLAYER)) { winner = HUMAN_PLAYER; return true; }
    if (checkWin(AI_PLAYER)) { winner = AI_PLAYER; return true; }
    if (isBoardFull()) { winner = EMPTY_SLOT; return true; }
    winner = ' ';
    return false;
}
// --- End Game Logic ---


// --- Modified getPlayerMove with colored prompt and error messages ---
int ConnectFour::getPlayerMove() {
    int col;
    string prompt = "Player " + Color::BOLD_RED + string(1, HUMAN_PLAYER) + Color::RESET +
                    ", enter column (" + Color::CYAN + "0-" + to_string(COLS - 1) + Color::RESET + "): ";
    while (true) {
        // Use the utility function for robust, ranged input
        col = getIntInput(prompt, 0, COLS - 1);

        // Check if the chosen column is valid (not full)
        if (isValidColumn(col)) {
            return col; // Valid column chosen
        } else {
            // Provide specific feedback if column is full
            cout << Color::BOLD_RED << "Invalid move. Column " << col
                      << " is full. Please choose another column.\n" << Color::RESET;
        }
    }
}
// --- End getPlayerMove ---


// --- AI Implementation (Minimax, Heuristics) - unchanged ---
ConnectFour::Move ConnectFour::findBestMove() {
    int bestScore = numeric_limits<int>::min();
    Move bestMove;
    bestMove.col = -1;
    int alpha = numeric_limits<int>::min();
    int beta = numeric_limits<int>::max();

    vector<int> possibleMoves;
    for (int c = 0; c < COLS; ++c) {
        if (isValidColumn(c)) {
            possibleMoves.push_back(c);
        }
    }

    // Basic heuristic: Check for immediate win first
    for (int c : possibleMoves) {
         int r = getNextOpenRow(c);
         board[r][c] = AI_PLAYER;
         if (checkWin(AI_PLAYER)) {
             board[r][c] = EMPTY_SLOT; // Undo test move
             bestMove.col = c;
             bestMove.score = 1000000; // Assign immediate win highest score
             return bestMove;
         }
         board[r][c] = EMPTY_SLOT; // Undo test move
    }
     // Basic heuristic: Check for immediate block of opponent win
     for (int c : possibleMoves) {
         int r = getNextOpenRow(c);
         board[r][c] = HUMAN_PLAYER; // Temporarily place opponent's piece
         if (checkWin(HUMAN_PLAYER)) {
             board[r][c] = EMPTY_SLOT; // Undo test move
             bestMove.col = c; // Choose this column to block
             bestScore = 90000; // High score for blocking, but less than winning
             // Don't return immediately, check if other moves might be better via minimax
             goto evaluate_other_moves; // Jump to minimax evaluation
         }
         board[r][c] = EMPTY_SLOT; // Undo test move
     }


evaluate_other_moves:
    // If no immediate win/block found or if block score needs comparison
    int currentBestScore = (bestMove.col != -1) ? bestScore : numeric_limits<int>::min();
    if(bestMove.col == -1) bestScore = numeric_limits<int>::min(); // Reset if no block was found

    for (int c : possibleMoves) {
        int r = getNextOpenRow(c);
        board[r][c] = AI_PLAYER;
        int score = minimaxAlphaBeta(0, numeric_limits<int>::min(), numeric_limits<int>::max(), false); // Reset alpha/beta for each top-level move
        board[r][c] = EMPTY_SLOT; // Undo move

        if (bestMove.col == -1 || score > bestScore) { // If first move or better score
             bestScore = score;
             bestMove.col = c;
        }
    }

     // Fallback if something went wrong
     if (bestMove.col == -1 && !possibleMoves.empty()) {
         bestMove.col = possibleMoves[possibleMoves.size() / 2]; // Prefer center column as fallback
         if (!isValidColumn(bestMove.col)) bestMove.col = possibleMoves[0]; // Or just first available
     }

    bestMove.score = bestScore;
    return bestMove;
}

int ConnectFour::minimaxAlphaBeta(int depth, int alpha, int beta, bool isMaximizingPlayer) {
    char winner;
    if (checkGameOver(winner)) {
        if (winner == AI_PLAYER) return 100000 - depth; // Faster wins are better
        if (winner == HUMAN_PLAYER) return -100000 + depth; // Slower losses are better
        return 0; // Draw
    }
    if (depth >= MAX_DEPTH) return evaluateBoard();

    vector<int> possibleMoves;
    for (int c = 0; c < COLS; ++c) if (isValidColumn(c)) possibleMoves.push_back(c);

    if (isMaximizingPlayer) { // AI's Turn (O)
        int maxEval = numeric_limits<int>::min();
        for (int c : possibleMoves) {
            int r = getNextOpenRow(c);
            board[r][c] = AI_PLAYER;
            int eval = minimaxAlphaBeta(depth + 1, alpha, beta, false);
            board[r][c] = EMPTY_SLOT;
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break; // Beta cutoff
        }
        return maxEval;
    } else { // Human's Turn (X)
        int minEval = numeric_limits<int>::max();
        for (int c : possibleMoves) {
            int r = getNextOpenRow(c);
            board[r][c] = HUMAN_PLAYER;
            int eval = minimaxAlphaBeta(depth + 1, alpha, beta, true);
            board[r][c] = EMPTY_SLOT;
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break; // Alpha cutoff
        }
        return minEval;
    }
}

int ConnectFour::evaluateBoard() const {
    int score = 0;
    // Center column control heuristic
    for(int r = 0; r < ROWS; ++r) {
        if (board[r][COLS / 2] == AI_PLAYER) score += 3;
        else if (board[r][COLS / 2] == HUMAN_PLAYER) score -= 3;
    }

    // Horizontal, Vertical, Diagonal scoring
    auto checkLines = [&](int dr, int dc) {
        int current_score = 0;
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                vector<char> line;
                bool possible = true;
                for (int k = 0; k < WIN_LENGTH; ++k) {
                    int nr = r + k * dr;
                    int nc = c + k * dc;
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {
                         line.push_back(board[nr][nc]);
                    } else { possible = false; break; }
                }
                 if (possible && line.size() == WIN_LENGTH) {
                     current_score += scoreLine(line, AI_PLAYER);
                     current_score -= scoreLine(line, HUMAN_PLAYER); // Subtract opponent's score potential
                 }
            }
        }
        return current_score;
    };

    score += checkLines(0, 1); // Horizontal
    score += checkLines(1, 0); // Vertical
    score += checkLines(1, 1); // Positive Diagonal (/)

    // Negative Diagonal (\) - Adjusted check
     int neg_diag_score = 0;
     for (int r = WIN_LENGTH - 1; r < ROWS; ++r) {
         for (int c = 0; c <= COLS - WIN_LENGTH; ++c) {
             vector<char> line;
             for (int k = 0; k < WIN_LENGTH; ++k) line.push_back(board[r-k][c+k]);
             neg_diag_score += scoreLine(line, AI_PLAYER);
             neg_diag_score -= scoreLine(line, HUMAN_PLAYER);
         }
     }
     score += neg_diag_score;

    return score;
}

int ConnectFour::scoreLine(const vector<char>& line, char player) const {
    int score = 0;
    int playerCount = 0;
    int emptyCount = 0;
    char opponent = (player == AI_PLAYER) ? HUMAN_PLAYER : AI_PLAYER;
    int opponentCount = 0;

    for (char slot : line) {
        if (slot == player) playerCount++;
        else if (slot == EMPTY_SLOT) emptyCount++;
        else opponentCount++;
    }
    // Give no score if opponent is blocking the line
    if (opponentCount > 0) return 0;

    // Assign scores based on player's pieces
    if (playerCount == 4) score = 10000;  // Win
    else if (playerCount == 3 && emptyCount == 1) score = 100;  // Threaten win (3 in a row)
    else if (playerCount == 2 && emptyCount == 2) score = 10;   // Potential (2 in a row)
    else if (playerCount == 1 && emptyCount == 3) score = 1;    // Slight potential

    return score;
}
// --- End AI Implementation ---


// --- Modified Main Play Loop with UI enhancements ---
void ConnectFour::play() {
    initializeBoard();
    char currentPlayer = HUMAN_PLAYER;
    char winner = ' ';
    bool gameOver = false;

    while (!gameOver) {
        clearScreen();
        cout << Color::BOLD_YELLOW << "=== Connect Four ===\n" << Color::RESET;
        displayBoard(); // Display board with new UI

        string status;
        if (currentPlayer == HUMAN_PLAYER) {
            status = "Your turn (Player " + Color::BOLD_RED + HUMAN_PLAYER + Color::RESET + ")";
            cout << status << "\n";
            int col = getPlayerMove(); // Get validated move
            if (!dropPiece(col, HUMAN_PLAYER)) {
                // This block should ideally not be reached due to getPlayerMove validation
                 cout << Color::BOLD_RED << "Internal Error: Failed to drop piece in valid column " << col << ".\n" << Color::RESET;
                 this_thread::sleep_for(chrono::seconds(2));
                 continue; // Allow trying again or investigate error
            }
        } else { // AI_PLAYER's turn
            status = "AI's turn (" + Color::BOLD_YELLOW + AI_PLAYER + Color::RESET + ")... Thinking...";
            cout << status << "\n";
            cout.flush(); // Ensure message displays before potential delay
            this_thread::sleep_for(chrono::milliseconds(500)); // Simulate thinking

            Move aiMove = findBestMove();

            if (aiMove.col != -1 && dropPiece(aiMove.col, AI_PLAYER)) {
                // Optional: Give feedback on AI's move right after it happens
                // clearScreen();
                // cout << Color::BOLD_YELLOW << "=== Connect Four ===\n" << Color::RESET;
                // displayBoard();
                // cout << "AI dropped a piece in column: " << Color::CYAN << aiMove.col << Color::RESET << "\n";
                // this_thread::sleep_for(chrono::milliseconds(800)); // Pause to see AI move
            } else {
                 // Handle AI failure or full board scenario
                 cout << Color::BOLD_RED << "AI error or board full: AI could not make a move.\n" << Color::RESET;
                 gameOver = true; // Force end if AI fails
                 this_thread::sleep_for(chrono::seconds(2));
            }
        }

        // Check game status after the move
        gameOver = checkGameOver(winner);

        // Switch player if game is not over
        if (!gameOver) {
            currentPlayer = (currentPlayer == HUMAN_PLAYER) ? AI_PLAYER : HUMAN_PLAYER;
        }
    } // End of game loop

    // --- Game Over Section ---
    clearScreen();
    cout << Color::BOLD_YELLOW << "=== Connect Four: Game Over ===\n" << Color::RESET;
    displayBoard(); // Show the final board state

    // Display colored result message
    if (winner == HUMAN_PLAYER) {
        cout << Color::BOLD_GREEN << "*** Congratulations! You (" << HUMAN_PLAYER << ") win! ***\n" << Color::RESET;
    } else if (winner == AI_PLAYER) {
        cout << Color::BOLD_RED << "*** AI Player (" << AI_PLAYER << ") wins! ***\n" << Color::RESET;
    } else { // Draw
        cout << Color::BOLD_YELLOW << "*** It's a Draw! ***\n" << Color::RESET;
    }
    // --- End Game Over Section ---

    // Pause handled by main.cpp loop
}
// --- End play() function ---