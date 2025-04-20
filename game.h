#ifndef GAME_H
#define GAME_H

#include <string>

// Abstract base class for all games
class Game {
public:
    // Pure virtual function - must be implemented by derived classes
    virtual void play() = 0;

    // Virtual destructor is important for base classes with virtual functions
    virtual ~Game() = default;

    // Optional: Could add a common name getter if needed
    virtual std::string getName() const = 0;
};

#endif // GAME_H