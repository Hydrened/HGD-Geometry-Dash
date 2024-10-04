#ifndef MENU_H
#define MENU_H

#include "game.h"
#include "utils.h"
class Game;

class Menu {
private:
    Game* game;

    int levelIndex = 0;

    void renderMainMenu();
    void renderLevelMenu();

public:
    Menu(Game* game);
    ~Menu();

    void update();
    void render();

    int getLevelIndex() const;

    void incrLevelIndex(int incr);
};

#endif
