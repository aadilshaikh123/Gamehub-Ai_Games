#ifndef NIM_H
#define NIM_H

#include "game.h"
#include <vector>
#include <string>

class Nim : public Game {
public:
    // Allow customizing pile setup
    Nim(std::vector<int> initial_piles = {3, 4, 5});
    void play() override;
    std::string getName() const override { return "Nim"; }
    virtual ~Nim() = default;

private:
    std::vector<int> piles;
    static const char HUMAN_PLAYER = 'H'; // Just identifiers for turns
    static const char AI_PLAYER = 'A';

    void displayPiles() const;
    bool isValidMove(int pileIndex, int numToRemove) const;
    bool isGameOver() const;
    void getPlayerMove(int& pileIndex, int& numToRemove);

    // AI using Nim-Sum strategy
    struct Move {
        int pileIndex = -1;
        int numToRemove = -1;
    };
    int calculateNimSum() const;
    Move findBestMove();
};

#endif // NIM_H