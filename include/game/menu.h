#ifndef MENU_H
#define MENU_H

#include "game.h"
#include "utils.h"
class Game;

class Menu {
private:
    Game* game;

    int levelIndex = 0;

public:
    Menu(Game* game);
    ~Menu();

    void update();
    void render();
    void renderMainMenu();
    void renderLevelMenu();

    int getLevelIndex() const;

    void incrLevelIndex(int incr);
};

#endif
