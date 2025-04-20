#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// --- UI Enhancements ---
namespace Color {
    extern const std::string RESET;
    extern const std::string BLACK;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string BLUE;
    extern const std::string MAGENTA;
    extern const std::string CYAN;
    extern const std::string WHITE;
    extern const std::string BOLD_RED;
    extern const std::string BOLD_GREEN;
    extern const std::string BOLD_YELLOW;
    extern const std::string BOLD_BLUE;
    // Add more if needed
}
// --- End UI Enhancements ---

// Function to clear the terminal screen (platform-dependent)
void clearScreen();

// Function to pause execution and wait for Enter key press
void pressEnterToContinue();

// Function to get validated integer input within a range
int getIntInput(const std::string& prompt, int minVal = -2147483648, int maxVal = 2147483647);

#endif // UTILS_H