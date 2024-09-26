#include "menu.h"

// INIT
Menu::Menu(Game* g) : game(g) {

}

// CLEANUP
Menu::~Menu() {
    
}

// UPDATE
void Menu::update() {
    GameState state = game->getState();
    switch (state.main) {

    }
}

// RENDER
void Menu::render() {
    GameState state = game->getState();

    switch (state.main) {
        case MAIN_MENU: renderMainMenu(); break;
        case LEVEL_MENU: renderLevelMenu(); break;
    }
}

void Menu::renderMainMenu() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_GraphicObject* temp = new H2DE_GraphicObject();
    temp->type = TEXT;
    temp->text = "Press space to go to the level menu";
    temp->font = "Helvetica.ttf";
    temp->pos = { 100, 100 };
    temp->size = { 800, 70 };
    temp->color = { 255, 255, 255, 255 };
    H2DE_AddGraphicObject(engine, temp);
}

void Menu::renderLevelMenu() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_GraphicObject* temp = new H2DE_GraphicObject();
    temp->type = TEXT;
    temp->text = "Press space play Stereo Madness";
    temp->font = "Helvetica.ttf";
    temp->pos = { 100, 100 };
    temp->size = { 800, 70 };
    temp->color = { 255, 255, 255, 255 };
    H2DE_AddGraphicObject(engine, temp);
}

// GETTER
int Menu::getLevelIndex() {
    return levelIndex;
}
