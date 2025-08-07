#include "game.h"

#include "events.h"
#include "menu/menu.h"
#include "level/level.h"

// INIT
Game::Game() : save(new Save()) {
    initEngine();
    initEvents();
    initData();
    initLoadingScreen();
}

void Game::initEngine() {
    H2DE_EngineData engineData = H2DE_EngineData();

    engineData.window.title = "Geometry Dash 1.0";
    engineData.window.fps = 60;
    engineData.window.pos = { 10, 30 };
    engineData.window.size = { 1520, 855 };
    engineData.window.ratio = H2DE_WINDOW_RATIO_16_9;
    engineData.window.resizable = false;
    engineData.window.saveState = true;
    engineData.window.fullscreen = false;

    engineData.camera.gameWidth = 19.0f;
    engineData.camera.interfaceWidth = engineData.camera.gameWidth;
    engineData.camera.smoothing = 0.0f;
    engineData.camera.yOrigin = H2DE_FACE_BOTTOM;
    engineData.camera.grid = false;

    engine = H2DE::createEngine(engineData);

    engine->setHandleEventCall([this](SDL_Event event) {
        events->handle(event);
    });

    engine->setUpdateCall([this]() {
        update();
    });

    engine->getWindow()->setIcon("game-icon.png");
    engine->debugObjects(false);
}

void Game::initEvents() {
    events = new Events(this);
}

void Game::initData() {
    data = new Data(this);
}

void Game::initLoadingScreen() {
    loadingScreen = new LoadingScreenMenu(this, MENU_ID_LOADING_SCREEN_MENU);
}

// CLEANUP
Game::~Game() {
    if (loadingScreen != nullptr) {
        delete loadingScreen;
    }

    if (menu != nullptr) {
        delete menu;
    }

    if (level != nullptr) {
        delete level;
    }

    delete data;
    delete events;
    delete save;

    H2DE::destroyEngine(engine);
}

// RUN
void Game::run() {
    engine->run();
}

void Game::stop() {
    engine->stop();
}

// UPDATE
void Game::update() {
    switch (state) {
        case GAME_STATE_MENU: menu->update(); break;
        case GAME_STATE_LEVEL: level->update(); break;
        default: break;
    }
}

// ACTIONS
void Game::openMenu(MenuID id) {
    bool needToCloseMenu = (menu != nullptr);
    bool needToCloseLevel = (level != nullptr);
    bool needClosingTransition = (needToCloseMenu || needToCloseLevel);

    if (needClosingTransition) {
        if (needToCloseMenu) {

            menu->close([this, id]() {
                menu = nullptr;
                menu = createMenu(id);
                state = GAME_STATE_MENU;
            });

        } else if (needToCloseLevel) {

            level->close([this, id]() {
                level = nullptr;
                menu = createMenu(id);
                state = GAME_STATE_MENU;
            });
        }
        
    } else {
        menu = createMenu(id);
        state = GAME_STATE_MENU;
    }
}

Menu* Game::createMenu(MenuID id) {
    switch (id) {
        case MENU_ID_MAIN_MENU: return new MainMenu(this, id);
        case MENU_ID_LEVEL_MENU: return new LevelMenu(this, id);
        case MENU_ID_ICON_MENU: return new IconMenu(this, id);
        default: return nullptr;
    }
}

void Game::openLevel(LevelID id) {
    static H2DE_Audio* audio = engine->getAudio();

    bool isALevelOpened = (level != nullptr);
    if (isALevelOpened) {
        return;
    }

    audio->stopSong();
    audio->playSfx("play-level.ogg", 0, false);
    
    bool needToCloseMenu = (menu != nullptr);
    if (needToCloseMenu) {

        menu->close([this, id]() {
            // temp
            Checkpoint checkpoint = Checkpoint();
            checkpoint.translate = { 0.0f, 0.0f };
            
            menu = nullptr;
            level = new Level(this, id);
            state = GAME_STATE_LEVEL;
        });

    } else {
        level = new Level(this, id);
        state = GAME_STATE_LEVEL;
    }
}
