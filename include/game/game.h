#ifndef GAME_H
#define GAME_H

#include <H2DE/H2DE.h>
#include "utils.h"
#include "data.h"
#include "functions.h"
#include "menu.h"
#include "level.h"
#include "save.h"
class Data;
class Menu;
class Level;
class Save;

class Game {
private:
    H2DE_Engine* engine = nullptr;

    Data* data = nullptr;
    Menu* menu = nullptr;
    Level* level = nullptr;

    GameState state = LOADING_ASSETS;
    bool inTransition = true;
    Save* save;
    
    H2DE_BasicObject* transition = nullptr;

    void initEngine();
    void initData();
    void initTransition();

    void destroySave();
    void destroyTransition();
    void destroyMenu();
    void destroyData();
    void destroyEngine();

    void event_keydown(SDL_Keycode keycode);
    void event_keydown_menu(SDL_Keycode keycode);
    void event_keydown_level(SDL_Keycode keycode);
    void event_keyup(SDL_Keycode keycode);
    void event_keyup_level(SDL_Keycode keycode);
    void event_button_down(Uint8 button);
    void event_button_down_level(Uint8 button);
    void event_button_up(Uint8 button);
    void event_button_up_level(Uint8 button);

    void closeMenu();
    void closeLevel();
    void transitionIn();
    void transitionOut();

public:
    Game();
    ~Game();

    void run();
 
    void handleEvents(SDL_Event event);

    void update();
    void updateCamera(int speed) const;

    void openMenu(MenuID id, const std::function<void()>& call);
    void openLevel(int id, const std::function<void()>& call);

    void destroyObjects(const std::vector<H2DE_Object*>& objects) const;

    inline H2DE_Engine* getEngine() const { return engine; }
    inline const Data* getData() const { return data; }
    inline const Save* getSave() const { return save; }

    H2DE_LevelPos convertToCamPos(const H2DE_LevelPos& pos) const;
    H2DE_LevelPos convertToLevelPos(const H2DE_LevelPos& pos, const H2DE_LevelSize& size) const;
    H2DE_LevelPos convertToGdPos(const H2DE_LevelPos& pos, const H2DE_LevelSize& size) const;
    H2DE_LevelRect convertToLevelRect(const H2DE_LevelRect& rect) const;
};

#endif
