#pragma once

#include <H2DE/H2DE.h>
#include "utils.h"
#include "data.h"
#include "save.h"
class Data;
class Events;
class Save;
class LoadingScreenMenu;
class Menu;
class Level;

class Game {
public:
    Game();
    ~Game();

    void run();
    void stop();

    void openMenu(MenuID id);
    void openLevel(LevelID id);
    
    inline H2DE_Engine* getEngine() const {
        return engine;
    }
    inline const Data* getData() const {
        return data;
    }
    inline Save* getSave() const {
        return save;
    }

    friend class LoadingScreenMenu;
    friend class Transition;
    friend class Events;
    friend class Menu;

private:
    H2DE_Engine* engine = nullptr;
    
    Data* data = nullptr;
    Events* events = nullptr;
    Save* save = nullptr;

    LoadingScreenMenu* loadingScreen = nullptr;
    Menu* menu = nullptr;
    Level* level = nullptr;

    GameState state = GAME_STATE_LOADING_SCREEN;
    bool inTransition = true;

    void initEngine();
    void initEvents();
    void initData();
    void initLoadingScreen();

    void update();

    Menu* createMenu(MenuID id);
};
