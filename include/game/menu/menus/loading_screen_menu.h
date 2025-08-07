#pragma once

#include "menu/menu.h"
class Game;
class Scenery;

class LoadingScreenMenu : public Menu {
public:
    LoadingScreenMenu(Game* game, MenuID id);
    ~LoadingScreenMenu() override;

    void update() override;

private:
    Scenery* scenery = nullptr;
    H2DE_BarObject* loadingBar = nullptr;

    void initSyncLoad();
    void initScenery();
    void initCamera() override;
    void initLoadingBar();
    void initAsyncLoad();

    void destroyScenery();
    void destroyLoadingBar();
};
