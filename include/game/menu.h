#ifndef MENU_H
#define MENU_H

#include "game.h"
#include "utils.h"
class Game;

class Menu {
private:
    Game* game;

    int levelIndex = 0;
    MenuIcon* icon = nullptr;

    LevelPos backgroundPos;
    Color backgroundColor;
    Color groundColor;

    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

    void updateIcon();
    void renderMainMenu();
    void renderLevelMenu();
    void renderIcon();
    void renderMainTexture();
    void renderSecondTexture();

    void spawnIcon();

public:
    Menu(Game* game);
    ~Menu();

    void update();
    void render();
    void resetMainMenu();

    int getLevelIndex() const;

    void incrLevelIndex(int incr);
};

#endif
