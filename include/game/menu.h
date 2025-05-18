#ifndef MENU_H
#define MENU_H

#include "game.h"
#include "scenery.h"
#include "player.h"
class Game;
class Scenery;
class Player;

class Menu {
private:
    Game* game;
    MenuID id;

    Scenery* scenery = nullptr;
    int mainMenuColorLoop = -1;

    std::unordered_map<MenuID, std::string> strMenus {
        { MAIN_MENU, "main menu" },
        { LEVEL_MENU, "level menu" },
    };
    std::vector<H2DE_Object*> objects = {};

    void createObjects();
    void createBarObject(const ObjectBuffer& buffer);
    void createBasicObject(const ObjectBuffer& buffer);
    void createButtonObject(const ObjectBuffer& buffer);
    void createTextObject(const ObjectBuffer& buffer);
    void createObjectInitBOD(const ObjectBuffer& buffer, const std::unordered_map<std::string, SurfaceDataBuffer>& surfaceDataBuffers, std::unordered_map<std::string, H2DE_Surface*>& surfaces);
    void initMainMenu();
    void playSong() const;

    void destroyScenery();
    void destroyMainMenuColorLoop();

public:
    Menu(Game* game, MenuID id);
    ~Menu();

    void update();

    void disableButtons() const;

    inline const MenuID getId() const { return id; }
};

#endif
