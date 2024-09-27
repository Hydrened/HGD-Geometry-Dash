#ifndef MEGAHCK_H
#define MEGAHCK_H

class Game;
#include "game.h"

class Megahack {
private:
    Game* game;
    std::unordered_map<std::string, Hack*> hacks;

    void loadHacks();
    void saveHacks();

public:
    Megahack(Game* game);
    ~Megahack();

    std::unordered_map<std::string, Hack*> getHacks();
};

#endif
