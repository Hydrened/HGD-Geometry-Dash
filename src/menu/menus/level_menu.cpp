#include "menu/menus/level_menu.h"

#include "level/scenery.h"

// INIT
LevelMenu::LevelMenu(Game* g, MenuID i) : Menu(g, i) {
    initCamera();
    initScenery();
}

void LevelMenu::initCamera() {
    static const H2DE_Translate& levelMenuCameraTranslate = game->getData()->getLevelMenuCameraTranslate();
    setCameraTranslate(levelMenuCameraTranslate);
}

void LevelMenu::initScenery() {
    scenery = new Scenery(game, 13, 1);
}

// CLEANUP
LevelMenu::~LevelMenu() {
    delete scenery;
}

// UPDATE
void LevelMenu::update() {

}

// ACTIONS
void LevelMenu::incrementLevelID(int8_t increment) {
    levelIndex += increment;

    if (levelIndex < 0) {
        levelIndex = nbLevels - 1;

    } else if (levelIndex >= (int8_t)nbLevels) {
        levelIndex = 0;
    }
}
