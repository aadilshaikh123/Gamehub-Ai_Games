#include <iostream>
#include <vector>
#include <memory> // For std::unique_ptr
#include <limits>
#include <string>

// Include game headers
#include "utils.h" // For clearScreen, pressEnterToContinue
#include "game.h" // Abstract base class
#include "tictactoe.h"
#include "connectfour.h"
#include "nim.h"
#include "mazesolver.h"

int main() {
    // Use smart pointers to manage game objects polymorphically
    std::vector<std::unique_ptr<Game>> games;
    games.push_back(std::make_unique<TicTacToe>());
    games.push_back(std::make_unique<ConnectFour>());
    games.push_back(std::make_unique<Nim>()); // Default Nim piles
    // games.push_back(std::make_unique<Nim>(std::vector<int>{1, 2, 3, 4})); // Example custom Nim piles
    games.push_back(std::make_unique<MazeSolver>("maze.txt")); // Load from file

    int choice = 0;
    const int exitChoice = games.size() + 1;

    do {
        clearScreen();
        std::cout << "===========================\n";
        std::cout << "    AI Game Hub Menu       \n";
        std::cout << "===========================\n";

        // Display game options dynamically
        for (size_t i = 0; i < games.size(); ++i) {
            std::cout << i + 1 << ". Play " << games[i]->getName() << "\n";
        }
        std::cout << exitChoice << ". Exit\n";
        std::cout << "===========================\n";

        choice = getIntInput("Enter your choice: ", 1, exitChoice);


        if (choice >= 1 && choice <= static_cast<int>(games.size())) {
            // Valid game choice
            games[choice - 1]->play(); // Polymorphic call to the specific game's play() method
            pressEnterToContinue(); // Pause after the game finishes before showing menu
        } else if (choice == exitChoice) {
            std::cout << "\nExiting Game Hub. Goodbye!\n";
        } else {
            // getIntInput should prevent this, but as a failsafe
            std::cout << "\nInvalid choice.\n";
             pressEnterToContinue();
        }

    } while (choice != exitChoice);

    return 0;
}