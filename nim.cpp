#include "nim.h"
#include "utils.h"      // Includes Color namespace
#include <iostream>
#include <vector>
#include <numeric>      // For accumulate
#include <limits>
#include <string>       // For string manipulation
#include <thread>       // For this_thread::sleep_for
#include <chrono>       // For chrono::milliseconds

// Add this line after includes
using namespace std;

// --- Constructor and Game Logic (isValidMove, isGameOver) - unchanged ---
Nim::Nim(vector<int> initial_piles) : piles(initial_piles) {
    // Ensure no negative pile sizes initially
    for(int& p : piles) {
        if (p < 0) p = 0;
    }
     // Ensure there's at least one pile if input was empty
    if (piles.empty()) {
         piles = {3, 4, 5}; // Add a default if empty
    }
}

bool Nim::isValidMove(int pileIndex, int numToRemove) const {
    return pileIndex >= 0 && pileIndex < piles.size() && // Valid pile index
           numToRemove > 0 &&                           // Must remove at least one
           numToRemove <= piles[pileIndex];             // Cannot remove more than available
}

bool Nim::isGameOver() const {
    // Game is over if all piles are empty (sum is 0)
    // Check using accumulate or a simple loop
    // return accumulate(piles.begin(), piles.end(), 0) == 0;
    for (int count : piles) {
        if (count > 0) return false; // Found a non-empty pile
    }
    return true; // All piles are empty
}
// --- End Game Logic ---


// --- Modified displayPiles with better UI ---
void Nim::displayPiles() const {
    cout << "\n" << Color::WHITE << "Current Piles:" << Color::RESET << "\n";
    cout << Color::WHITE << "~~~~~~~~~~~~~~~~~" << Color::RESET << "\n"; // Separator
    for (size_t i = 0; i < piles.size(); ++i) {
        // Display Pile index clearly
        cout << " Pile " << Color::CYAN << i << Color::RESET << ": ";
        // Display count in Yellow
        cout << "[" << Color::YELLOW << piles[i] << Color::RESET << "] ";
        // Display items using a character (e.g., 'O' or '|')
        cout << "(";
        for (int j = 0; j < piles[i]; ++j) {
            cout << Color::GREEN << "O" << Color::RESET; // Green 'O' for items
            if (j < piles[i] - 1) cout << " "; // Add space between items for clarity
        }
        cout << ")\n";
    }
    cout << Color::WHITE << "~~~~~~~~~~~~~~~~~" << Color::RESET << "\n\n"; // Separator
}
// --- End displayPiles ---


// --- Modified getPlayerMove with colored prompts and better validation feedback ---
void Nim::getPlayerMove(int& pileIndex, int& numToRemove) {
     while (true) {
        cout << Color::BOLD_GREEN << "Your turn." << Color::RESET << "\n";

        // Get pile index using the utility function
        pileIndex = getIntInput(" Enter pile index to remove from: ", 0, piles.size() - 1);

        // Check if the selected pile index is valid (should be guaranteed by getIntInput range)
        // But add check for empty pile specifically
        if (pileIndex < 0 || pileIndex >= piles.size()) {
             cout << Color::BOLD_RED << " Error: Invalid pile index selected.\n" << Color::RESET; // Should not happen
             continue;
        }
        if (piles[pileIndex] == 0) {
            cout << Color::BOLD_RED << " Pile " << pileIndex << " is already empty. Please choose another pile.\n" << Color::RESET;
            continue; // Ask for input again
        }

        // Get number to remove, using pile size as the max limit
        string prompt = " Enter number of items to remove from pile " + Color::CYAN + to_string(pileIndex) +
                        Color::RESET + " (" + Color::YELLOW + "1-" + to_string(piles[pileIndex]) + Color::RESET + "): ";
        numToRemove = getIntInput(prompt, 1, piles[pileIndex]);

        // Validation is now handled by getIntInput, isValidMove check is redundant here
        break; // Valid move parameters obtained

     } // End while loop
}
// --- End getPlayerMove ---


// --- AI Nim-Sum Strategy (logic unchanged) ---
int Nim::calculateNimSum() const {
    int nimSum = 0;
    for (int pileSize : piles) {
        nimSum ^= pileSize; // Bitwise XOR
    }
    return nimSum;
}

Nim::Move Nim::findBestMove() {
    int nimSum = calculateNimSum();
    Move bestMove;
    bestMove.pileIndex = -1; // Initialize to invalid
    bestMove.numToRemove = -1;

    if (nimSum != 0) {
        // Try to find a move to make nimSum zero (winning strategy)
        for (size_t i = 0; i < piles.size(); ++i) {
            if (piles[i] > 0) { // Only consider non-empty piles
                int targetSize = piles[i] ^ nimSum;
                if (targetSize < piles[i]) {
                    bestMove.pileIndex = i;
                    bestMove.numToRemove = piles[i] - targetSize;
                    return bestMove; // Found optimal move
                }
            }
        }
    }

    // If nimSum is 0 (losing position) or failed to find the optimal move (shouldn't happen)
    // Make a default move: remove 1 from the first non-empty pile found.
    for (size_t i = 0; i < piles.size(); ++i) {
        if (piles[i] > 0) {
            bestMove.pileIndex = i;
            bestMove.numToRemove = 1;
            break; // Take the first valid default move
        }
    }

    return bestMove; // Return the best (or default) move found
}
// --- End AI ---


// --- Modified Main Play Loop with UI enhancements ---
void Nim::play() {
    // Ensure piles are valid at the start
    if (piles.empty() || isGameOver()) {
         cout << Color::BOLD_RED << "Starting Nim game with empty or invalid piles. Resetting to default {3, 4, 5}.\n" << Color::RESET;
         piles = {3, 4, 5}; // Use default piles
         this_thread::sleep_for(chrono::seconds(1)); // Pause to see message
    }

    // Determine starting player (e.g., Human)
    char currentPlayer = HUMAN_PLAYER;
    bool gameOver = false;

    while (!gameOver) {
        clearScreen();
        cout << Color::BOLD_YELLOW << "=== Nim Game ===\n" << Color::RESET;
        displayPiles(); // Show piles with enhanced UI

        string status;
        if (currentPlayer == HUMAN_PLAYER) {
            // Human player's turn
            status = Color::BOLD_GREEN + string("Your turn (Human)") + Color::RESET;
            cout << status << "\n";
            int pileIdx, numRemove;
            getPlayerMove(pileIdx, numRemove); // Get validated move

            // Apply the move (with safety check)
            if (pileIdx >= 0 && pileIdx < piles.size() && numRemove > 0 && numRemove <= piles[pileIdx]) {
                 piles[pileIdx] -= numRemove;
            } else {
                 // This should ideally not happen due to getPlayerMove validation
                 cout << Color::BOLD_RED << "Internal Error: Invalid move parameters ["
                           << pileIdx << ", " << numRemove << "]. Skipping turn.\n" << Color::RESET;
                 this_thread::sleep_for(chrono::seconds(2));
            }
        } else { // AI_PLAYER's turn
            status = Color::BOLD_BLUE + string("AI's turn") + Color::RESET + "... Thinking...";
            cout << status << "\n";
            cout.flush(); // Show message before delay
            this_thread::sleep_for(chrono::milliseconds(600)); // Simulate thinking

            Move aiMove = findBestMove(); // Get AI's move using Nim-Sum strategy

            // Announce and apply AI's move
            if (aiMove.pileIndex != -1 && aiMove.numToRemove > 0) {
                 cout << " AI removes " << Color::YELLOW << aiMove.numToRemove << Color::RESET
                           << " from pile " << Color::CYAN << aiMove.pileIndex << Color::RESET << ".\n";
                 if (aiMove.pileIndex >= 0 && aiMove.pileIndex < piles.size()) { // Safety check
                    piles[aiMove.pileIndex] -= aiMove.numToRemove;
                 } else {
                     cout << Color::BOLD_RED << "Internal Error: AI chose invalid pile index " << aiMove.pileIndex << ".\n" << Color::RESET;
                 }
                 this_thread::sleep_for(chrono::milliseconds(900)); // Pause slightly to let user see AI move
            } else {
                // This should only happen if game is already over when AI turn starts
                 cout << Color::BOLD_RED << "AI cannot make a move (Game should be over?).\n" << Color::RESET;
                 gameOver = true; // Force end if AI fails to find a move on a non-finished board
                 this_thread::sleep_for(chrono::seconds(2));
            }
        }

        // Check if the game has ended *after* the current player's move
        if (!gameOver) { // Avoid re-checking if AI error already set gameOver
             gameOver = isGameOver();
        }


        if (gameOver) {
            // Game just ended, display final result
            clearScreen();
            cout << Color::BOLD_YELLOW << "=== Nim Game: Game Over ===\n" << Color::RESET;
            displayPiles(); // Show final empty piles
            // Winner is the player whose turn *it wasn't* when gameOver became true
            if (currentPlayer == HUMAN_PLAYER) { // Human made the last move
                cout << Color::BOLD_GREEN << "*** Congratulations! You win! ***\n" << Color::RESET;
            } else { // AI made the last move
                 cout << Color::BOLD_RED << "*** AI Player wins! ***\n" << Color::RESET;
            }
        } else {
            // Switch player for the next turn
            currentPlayer = (currentPlayer == HUMAN_PLAYER) ? AI_PLAYER : HUMAN_PLAYER;
        }

    } // End of game loop

    // Pause handled by main.cpp loop
}
// --- End play() function ---