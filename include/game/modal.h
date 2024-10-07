#ifndef MODAL_H
#define MODAL_H

#include "game.h"

class Modal {
private:
    Game* game;
    GameState state;

    float scaleMultiplier = 0;
    H2DE_TimelineManager* tm = new H2DE_TimelineManager();

public:
    Modal(Game* game, GameState state);
    ~Modal();

    void update();
    void render();
};

#endif
