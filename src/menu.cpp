#include "menu.h"

// INIT
Menu::Menu(Game* g) : game(g) {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();

    camera->setPos(gameData->positions->camera, 0);
    backgroundPos = gameData->positions->background;
    H2DE_PlaySong(engine, "menu_loop.mp3", -1);
}

// CLEANUP
Menu::~Menu() {
    static H2DE_Engine* engine = game->getEngine();
    
    H2DE_PauseSong(engine);
}

// UPDATE
void Menu::update() {
    static GameData* gameData = game->getData();
    Camera* camera = game->getCamera();

    LevelPos camPos = camera->getPos();
    GameState state = game->getState();

    switch (state.main) {
        case MAIN_MENU:
            backgroundPos.x += (gameData->physics->speeds[1] * gameData->physics->backgroundRatio);
            camera->setPos({ camPos.x + gameData->physics->speeds[1], camPos.y }, 0);
            break;
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
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    // Level elements
    H2DE_GraphicObject* ground = new H2DE_GraphicObject();
    ground->type = IMAGE;
    ground->texture = "ground_1.png"; // replace => ground_1.png(last ground used)
    ground->pos = calculator->convertToPx(gameData->positions->botGround, gameData->sizes->ground, false, false);
    ground->size = calculator->convertToPx(gameData->sizes->ground);
    ground->color = { 0, 102, 255, 255 }; // replace => rgb effect
    ground->repeatX = true;
    ground->index = Zindex{G, 0}.getIndex();
    H2DE_AddGraphicObject(engine, ground);

    H2DE_GraphicObject* line = new H2DE_GraphicObject();
    line->type = IMAGE;
    line->texture = "line_1.png";
    line->pos = calculator->convertToPx({ gameData->offsets->botLine.x, gameData->positions->botGround.y + gameData->offsets->botLine.y }, gameData->sizes->line, true, false);
    line->size = calculator->convertToPx(gameData->sizes->line);
    line->color = { 255, 255, 255, 255 };
    line->index = Zindex{G, 1}.getIndex();
    H2DE_AddGraphicObject(engine, line);

    H2DE_GraphicObject* background = new H2DE_GraphicObject();
    background->type = IMAGE;
    background->texture = "background_1.png"; // replace => background_1.png(last background used)
    background->pos = calculator->convertToPx(backgroundPos, gameData->sizes->background, false, false);
    background->size = calculator->convertToPx(gameData->sizes->background);
    background->color = { 35, 108, 221, 255 }; // replace => rgb effect
    background->repeatX = true;
    background->index = Zindex{BG, 0}.getIndex();
    H2DE_AddGraphicObject(engine, background);

    // Menu elements
    H2DE_GraphicObject* gameTitle = new H2DE_GraphicObject();
    gameTitle->type = IMAGE;
    gameTitle->texture = "game-title.png";
    gameTitle->pos = calculator->convertToPxAbs(gameData->positions->gameTitle);
    gameTitle->size = calculator->convertToPxAbs(gameData->sizes->gameTitle);
    gameTitle->index = Zindex{UI, 0}.getIndex();
    H2DE_AddGraphicObject(engine, gameTitle);

    H2DE_GraphicObject* levelMenuButton = new H2DE_GraphicObject();
    levelMenuButton->type = IMAGE;
    levelMenuButton->texture = "level-menu-button.png";
    levelMenuButton->pos = calculator->convertToPxAbs(gameData->positions->levelMenuButton);
    levelMenuButton->size = calculator->convertToPxAbs(gameData->sizes->levelMenuButton);
    levelMenuButton->index = Zindex{UI, 0}.getIndex();
    levelMenuButton->onclick = [this]() {
        this->game->setState({ LEVEL_MENU, DEFAULT }, game->getTransitionDuration(500), NULL);
    };
    H2DE_AddGraphicObject(engine, levelMenuButton);

    H2DE_GraphicObject* shopMenuButton = new H2DE_GraphicObject();
    shopMenuButton->type = IMAGE;
    shopMenuButton->texture = "shop-menu-button.png";
    shopMenuButton->pos = calculator->convertToPxAbs(gameData->positions->shopMenuButton);
    shopMenuButton->size = calculator->convertToPxAbs(gameData->sizes->shopMenuButton);
    shopMenuButton->index = Zindex{UI, 0}.getIndex();
    H2DE_AddGraphicObject(engine, shopMenuButton);

    H2DE_GraphicObject* onlineMenuButton = new H2DE_GraphicObject();
    onlineMenuButton->type = IMAGE;
    onlineMenuButton->texture = "online-menu-button.png";
    onlineMenuButton->pos = calculator->convertToPxAbs(gameData->positions->onlineMenuButton);
    onlineMenuButton->size = calculator->convertToPxAbs(gameData->sizes->onlineMenuButton);
    onlineMenuButton->index = Zindex{UI, 0}.getIndex();
    onlineMenuButton->onclick = [this]() {
        throw std::runtime_error("Abuse pas");
    };
    H2DE_AddGraphicObject(engine, onlineMenuButton);

    H2DE_GraphicObject* closeGameButton = new H2DE_GraphicObject();
    closeGameButton->type = IMAGE;
    closeGameButton->texture = "close-game-button.png";
    closeGameButton->pos = calculator->convertToPxAbs(gameData->positions->closeGameButton);
    closeGameButton->size = calculator->convertToPxAbs(gameData->sizes->closeGameButton);
    closeGameButton->index = Zindex{UI, 0}.getIndex();
    closeGameButton->onclick = [this]() {
        game->openModal({ MAIN_MENU, MODAL_EXIT });
    };
    H2DE_AddGraphicObject(engine, closeGameButton);
}

void Menu::renderLevelMenu() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    H2DE_GraphicObject* ground = new H2DE_GraphicObject();
    ground->type = IMAGE;
    ground->texture = "ground_1.png"; // replace => ground_1.png(last ground used)
    ground->pos = calculator->convertToPx(gameData->positions->botGround, gameData->sizes->ground, false, false);
    ground->size = calculator->convertToPx(gameData->sizes->ground);
    ground->color = { 0, 102, 255, 255 }; // replace => rgb effect
    ground->repeatX = true;
    ground->index = Zindex{G, 0}.getIndex();
    H2DE_AddGraphicObject(engine, ground);
}

// GETTER
int Menu::getLevelIndex() const {
    return levelIndex;
}

// SETTER
void Menu::incrLevelIndex(int incr) {
    int max = 1;

    if (levelIndex + incr < 0) levelIndex = max;
    else if (levelIndex + incr > max) levelIndex = 0;
    else levelIndex += incr;
}
