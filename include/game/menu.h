#ifndef MENU_H
#define MENU_H

#include "game.h"
#include "utils.h"
class Game;

class Menu {
private:
    Game* game;

    int levelIndex = 0;
    LevelPos backgroundPos;
    MenuIcon* icon = nullptr;

    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

    void updateIcon();
    void renderMainMenu();
    void renderLevelMenu();
    void renderIcon();
    void renderMainTexture();
    void renderSecondTexture();

public:
    Menu(Game* game);
    ~Menu();

    void update();
    void render();

    int getLevelIndex() const;

    void incrLevelIndex(int incr);
};

#endif
