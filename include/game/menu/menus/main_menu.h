#pragma once

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
    H2DE_Timeline* colorLoop = nullptr;
    H2DE_Delay* playerLoop = nullptr;

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
