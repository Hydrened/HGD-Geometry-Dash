#ifndef GAME_H
#define GAME_H

#include <H2DE/H2DE.h>
#include "utils.h"
#include "data.h"
#include "save.h"
class Data;
class Events;
class Save;
class Menu;
class Level;

class Game {
public:
    Game();
    ~Game();

    void run();
    void stop();

    void openMenu(MenuID id);
    void openLevel(uint32_t id);
    
    inline H2DE_Engine* getEngine() const { return engine; }
    inline const Data* getData() const { return data; }
    inline Save* getSave() const { return save; }

    friend class MenuTransition;
    friend class Events;

private:
    H2DE_Engine* engine = nullptr;
    
    Data* data = nullptr;
    Events* events = nullptr;
    Save* save = nullptr;

    Menu* menu = nullptr;
    Level* level = nullptr;

    GameState state = GAME_STATE_LOADING_ASSETS;
    bool inMenuTransition = true;

    void initEngine();
    void initEvents();
    void initData();

    void update();

    Menu* createMenu(MenuID id);
};

#endif
