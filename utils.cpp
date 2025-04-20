#include "utils.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <string>

// --- UI Color Definitions ---
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD_RED = "\033[1;31m";
    const std::string BOLD_GREEN = "\033[1;32m";
    const std::string BOLD_YELLOW = "\033[1;33m";
    const std::string BOLD_BLUE = "\033[1;34m";
}
// --- End UI Color Definitions ---


// Clears the terminal screen (Platform-dependent)
// ... (clearScreen function remains the same) ...
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


// Pauses the program and waits for the user to press Enter
// ... (pressEnterToContinue function remains the same - use the robust one) ...
void pressEnterToContinue() {
    std::cout << "\n" << Color::YELLOW << "Press Enter to continue..." << Color::RESET;
    std::cout.flush();
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}


// Gets integer input with validation and range checking
// ... (getIntInput function remains the same - use the robust one) ...
int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << Color::CYAN << prompt << Color::RESET; // Add color to prompt
        std::cout.flush();

        if (!(std::cin >> value)) {
            std::cout << Color::BOLD_RED << "Invalid input. Please enter a number.\n" << Color::RESET;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else if (value < minVal || value > maxVal) {
             std::cout << Color::BOLD_RED << "Input out of range. Please enter a value between "
                       << minVal << " and " << maxVal << ".\n" << Color::RESET;
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
         else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}