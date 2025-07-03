#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "menu/menu.h"
class Game;
class Scenery;
class Player;

class MainMenu : public Menu {
public:
    MainMenu(Game* game, MenuID id);
    ~MainMenu() override;

    void update() override;

    void click(const H2DE_Translate& translate);

private:
    Scenery* scenery = nullptr;
    Player* player = nullptr;
    H2DE_TimelineID colorLoop = H2DE_INVALID_TIMELINE_ID;
    H2DE_DelayID playerLoop = H2DE_INVALID_DELAY_ID;

    void initCamera() override;
    void initScenery();
    void initColorLoop();
    void initPlayerLoop();

    void destroyScenery();
    void destroyPlayer();
    void destroyColorLoop();
    void destroyPlayerLoop();

    void updateCamera();
    void updateScenery();
    void updatePlayer();

    void randomSpawnDelay();
    void spawnPlayer();
};

#endif
