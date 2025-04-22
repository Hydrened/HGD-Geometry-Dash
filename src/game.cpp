#include "game.h"

// INIT
Game::Game() : save(new Save()) {
    initEngine();
    initData();
    initTransition();

    openMenu(MAIN_MENU, nullptr);

    H2DE_DebugObjects(engine, false);
    H2DE_SetSongVolume(engine, 10);
}

void Game::initEngine() {
    H2DE_EngineData engineData = H2DE_EngineData();

    engineData.window.title = "Geometry Dash";
    engineData.window.fps = 60;
    engineData.window.pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
    engineData.window.size = { 1280, 720 };
    engineData.window.resizable = true;
    engineData.window.ratio = H2DE_WINDOW_RATIO_16_9;
    engineData.window.saveState = false;
    engineData.window.fullscreen = false;

    engineData.camera.width = 19.0f;
    engineData.camera.defaultPos = { 0.0f, 0.0f };
    engineData.camera.smoothing = 0.0f;

    engine = H2DE_CreateEngine(engineData);

    H2DE_SetHandleEventsCall(engine, [this](SDL_Event event) {
        handleEvents(event);
    });

    H2DE_SetUpdateCall(engine, [this]() {
        update();
    });

    H2DE_LoadAssets(engine, "assets");
}

void Game::initData() {
    data = new Data(this);
}

void Game::initTransition() {
    static const H2DE_LevelSize& gameSize = data->getGameSize();

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = "white.png";
    sd.rect = H2DE_LevelPos{ 0.0f, 0.0f }.makeRect(gameSize);
    sd.color = { 0, 0, 0, 0 };
    sd.scaleMode = H2DE_SCALE_MODE_NEAREST;

    H2DE_Surface* surface = H2DE_CreateTexture(engine, sd, H2DE_TextureData());
    
    H2DE_ObjectData od = H2DE_ObjectData();
    od.pos = { 0.0f, 0.0f };
    od.size = gameSize;
    od.absolute = true;
    od.index = INT_MAX;

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = surface;

    transition = H2DE_CreateBasicObject(engine, od, bod);
}

// CLEANUP
Game::~Game() {
    destroySave();
    destroyTransition();
    destroyMenu();
    destroyData();
    destroyEngine();
}

void Game::destroySave() {
    if (save) {
        delete save;
    }
}

void Game::destroyTransition() {
    if (transition) {
        H2DE_DestroyObject(engine, transition);
    }
}

void Game::destroyMenu() {
    if (menu) {
        delete menu;
    }
}

void Game::destroyData() {
    if (data) {
        delete data;
    }
}

void Game::destroyEngine() {
    H2DE_DestroyEngine(engine);
}

// RUN
void Game::run() {
    H2DE_RunEngine(engine);
}

// EVENTS
void Game::handleEvents(SDL_Event event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            event_keydown(event.key.keysym.sym);
            break;

        case SDL_KEYUP:
            event_keyup(event.key.keysym.sym);
            break;

        case SDL_MOUSEBUTTONDOWN:
            event_button_down(event.button.button);
            break;

        case SDL_MOUSEBUTTONUP:
            event_button_up(event.button.button);
            break;

        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    if (state == LEVEL) {
                        H2DE_Pause(engine);
                    }
                    break;
            }
            break;

        default: break;
    }
}

void Game::event_keydown(SDL_Keycode keycode) {
    if (inTransition) {
        return;
    }

    switch (state) {
        case MENU: event_keydown_menu(keycode); break;
        case LEVEL: event_keydown_level(keycode); break;
        default: return;
    }
}

void Game::event_keydown_menu(SDL_Keycode keycode) {
    const MenuID id = menu->getId();

    switch (keycode) {
        case SDLK_ESCAPE: switch (id) {
            case MAIN_MENU: H2DE_StopEngine(engine); break;
            case LEVEL_MENU: openMenu(MAIN_MENU, [this]() {
                closeMenu();
            }); break;
            default: break;
        } break;

        case SDLK_SPACE: switch (id) {
            case MAIN_MENU: openMenu(LEVEL_MENU, [this]() {
                closeMenu();
            }); break;
            case LEVEL_MENU: openLevel(0, [this]() {
                H2DE_StopSong(engine);
                closeMenu();
            }); break;
            default: break;
        } break;

        default: break;
    }
}

void Game::event_keydown_level(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_ESCAPE:
            if (H2DE_IsPaused(engine)) {
                openMenu(LEVEL_MENU, [this](){
                    closeLevel();
                    H2DE_StopSong(engine);
                });
            } else {
                H2DE_Pause(engine);
            }
            break;

        case SDLK_SPACE:
            if (H2DE_IsPaused(engine)) {
                H2DE_Resume(engine);
            } else {
                level->getPlayer()->setMouseDown(true);
            }
            break;

        case SDLK_UP:
            callShortcut(SDLK_SPACE);
            break;

        default: break;
    }
}

void Game::event_keyup(SDL_Keycode keycode) {
    if (inTransition) {
        return;
    }

    switch (state) {
        case LEVEL: event_keyup_level(keycode); break;
        default: return;
    }
}

void Game::event_keyup_level(SDL_Keycode keycode) {
    switch (keycode) {
        case SDLK_SPACE:
            level->getPlayer()->setMouseDown(false);
            break;

        case SDLK_UP:
            level->getPlayer()->setMouseDown(false);
            break;

        default: break;
    }
}

void Game::event_button_down(Uint8 button) {
    switch (state) {
        case LEVEL:
            event_button_down_level(button);
            break;
    }
}

void Game::event_button_down_level(Uint8 button) {
    switch (button) {
        case 1:
            level->getPlayer()->setMouseDown(true);
            break;
    }
}

void Game::event_button_up(Uint8 button) {
    switch (state) {
        case LEVEL:
            event_button_up_level(button);
            break;
    }
}

void Game::event_button_up_level(Uint8 button) {
    switch (button) {
        case 1:
            level->getPlayer()->setMouseDown(false);
            break;
    }
}

// UPDATE
void Game::update() {
    if (menu) {
        menu->update();

    } else if (level) {
        level->update();
    }
}

void Game::updateCamera(int speed) const {
    H2DE_LevelPos camPos = H2DE_GetCameraPos(engine);
    camPos.x += data->getSpeed(speed);
    H2DE_SetCameraPos(engine, camPos);
}

// MENUS
void Game::openMenu(MenuID id, const std::function<void()>& call) {
    const int duration = save->getTransitionDuration();

    if (menu) {
        menu->disableButtons();
    }

    transitionIn();

    H2DE_Delay(engine, duration, [this, id, call]() {
        if (call) {
            call();
        }

        H2DE_Resume(engine);
        menu = new Menu(this, id);
        state = MENU;

        transitionOut();
    }, false);
}

void Game::closeMenu() {
    if (menu) {
        delete menu;
        menu = nullptr;
    }
}

void Game::transitionIn() {
    inTransition = true;
    H2DE_SetSurfaceColor(H2DE_GetBasicObjectSurface(transition, "main"), { 0, 0, 0, 255 }, save->getTransitionDuration(), H2DE_EASING_LINEAR, false);
}

void Game::transitionOut() {
    const int duration = save->getTransitionDuration();

    H2DE_SetSurfaceColor(H2DE_GetBasicObjectSurface(transition, "main"), { 0, 0, 0, 0 }, duration, H2DE_EASING_LINEAR, false);
    H2DE_Delay(engine, duration, [this]() {
        inTransition = false;
    }, false);
}

// LEVELS
void Game::openLevel(int id, const std::function<void()>& call) {
    const int duration = save->getTransitionDuration();

    transitionIn();
    H2DE_StopSong(engine);
    H2DE_PlaySfx(engine, "play-level.ogg", 0, false);

    H2DE_Delay(engine, duration, [this, id, call]() {
        if (call) {
            call();
        }

        H2DE_Resume(engine);
        level = new Level(this, id, nullptr);
        state = LEVEL;

        transitionOut();
    }, false);
}

void Game::closeLevel() {
    if (level) {
        H2DE_StopSong(engine);
        delete level;
        level = nullptr;
    }
}

// OBJECTS
void Game::destroyObjects(const std::vector<H2DE_Object*>& objects) const {
    if (!engine) {
        return;
    }

    for (H2DE_Object* object : objects) {
        if (object) {
            H2DE_DestroyObject(engine, object);
        }
    }
}

// GETTER
H2DE_Engine* Game::getEngine() const {
    return engine;
}

const Data* Game::getData() const {
    return data;
}

const Save* Game::getSave() const {
    return save;
}

// CONVERT
H2DE_LevelPos Game::convertToCamPos(const H2DE_LevelPos& pos) const {
    return convertToLevelPos(pos, H2DE_GetCameraSize(engine));
}

H2DE_LevelPos Game::convertToLevelPos(const H2DE_LevelPos& pos, const H2DE_LevelSize& size) const {
    H2DE_LevelPos res = pos;
    res.y *= -1;
    res.y -= size.y;

    return res;
}

H2DE_LevelPos Game::convertToGdPos(const H2DE_LevelPos& pos, const H2DE_LevelSize& size) const {
    H2DE_LevelPos res = pos;
    res.y += size.y;
    res.y *= -1;

    return res;
}

H2DE_LevelRect Game::convertToLevelRect(const H2DE_LevelRect& rect) const {
    H2DE_LevelRect res = rect;
    res.y *= -1;
    res.y -= rect.h;

    return res;
}
