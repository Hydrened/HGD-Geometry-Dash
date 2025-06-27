#ifndef LEVEL_MENU_H
#define LEVEL_MENU_H

#include "menu/menu.h"
class Game;
class Scenery;

class LevelMenu : public Menu {
public:
    LevelMenu(Game* game, MenuID id);
    ~LevelMenu() override;
    
    void update() override;

    inline void left() { incrementLevelID(-1); }
    inline void right() { incrementLevelID(1); }

    constexpr uint32_t getLevelIndex() const { return levelIndex; }

private:
    Scenery* scenery = nullptr;
    int levelIndex = 0;
    int nbLevels = 3; // temp

    void initCamera() override;
    void initScenery();

    void incrementLevelID(int8_t increment);
};

#endif
