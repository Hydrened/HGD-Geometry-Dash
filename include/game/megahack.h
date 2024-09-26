#ifndef MEGAHCK_H
#define MEGAHCK_H

class Game;
#include "game.h"

class Megahack {
private:
    Game* game;

    void loadData();

public:
    Megahack(Game* game);
    ~Megahack();
};

#endif
