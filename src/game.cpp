#include "game.h"

// INIT
Game::Game(int f, int argc, char** argv) : FPS(f) {
    SDL_WindowFlags flag = SDL_WINDOW_RESIZABLE;

    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-f") flag = SDL_WINDOW_FULLSCREEN; 
        else if (arg == "-d") debug = true;
    }

    createWindow(flag);
    H2DE_LoadAssets(engine, "assets");
    calculator = new Calculator(this);
    megahack = new Megahack(this);

    camera = new Camera(this);
    setState({ MAIN_MENU, DEFAULT });
    openMenu();

    H2DE_SetSongVolume(engine, 0); // temp
    H2DE_SetSFXVolume(engine, -1, 0); // temp
}

void Game::createWindow(SDL_WindowFlags flag) {
    bool fullscreen = (flag == SDL_WINDOW_FULLSCREEN);
    json* settings = H2DE_Json::read("data/settings.json");

    int x = (fullscreen) ? 0 : static_cast<int>((*settings)["window"]["x"]);
    int y = (fullscreen) ? 0 : static_cast<int>((*settings)["window"]["y"]);
    int w = (fullscreen) ? 1920 : static_cast<int>((*settings)["window"]["w"]);
    int h = (fullscreen) ? 1080 : static_cast<int>((*settings)["window"]["h"]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("Error creating window => SDL_Init failed: " + std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("Geometry Dash 1.0 (1.0.0)", x, y, w, h, flag);
    if (!window) {
        SDL_Quit();
        throw std::runtime_error("Error creating window => SDL_CreateWindow failed: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("Error creating window => SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
    }

    engine = H2DE_CreateEngine(renderer, w, h, FPS);
    if (!engine) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        throw std::runtime_error("Error creating window => H2DE_CreateEngine failed");
    }

    SDL_SetWindowMaximumSize(window, ((fullscreen) ? 1920 : 1600), ((fullscreen) ? 1080 : 900));
    H2DE_SetEngineMaximumSize(engine, ((fullscreen) ? 1920 : 1600), ((fullscreen) ? 1080 : 900));
}

// CLEANUP
Game::~Game() {
    saveSettings();
    delete data;
    delete camera;
    if (menu) delete menu;
    if (level) delete level;
    delete calculator;
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    H2DE_DestroyEngine(engine);
    SDL_Quit();
}

void Game::quit() {
    isRunning = false;
}

void Game::saveSettings() {
    bool fullscreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN);

    if (!fullscreen) {
        std::string SETTINGSpath = "data/settings.json";
        json* settings = H2DE_Json::read(SETTINGSpath);

        int winX, winY, winW, winH;
        SDL_GetWindowPosition(window, &winX, &winY);
        SDL_GetWindowSize(window, &winW, &winH);
        (*settings)["window"]["x"] = winX;
        (*settings)["window"]["y"] = winY;
        (*settings)["window"]["w"] = winW;
        (*settings)["window"]["h"] = winH;

        if (!H2DE_Json::write(SETTINGSpath, settings)) {
            throw std::runtime_error("Error updating settings => Writing settings failed");
        }
    }
}

// RUN
void Game::run() {
    const int timePerFrame = 1000 / FPS;
    Uint32 now, fpsTimer, strt, handleEventsTime, updateTime, renderTime = SDL_GetTicks();
    int frameTime, frameCounter = 0, currentFPS = 0;
    SDL_Event event;

    while (isRunning) {
        now = SDL_GetTicks();
        strt = SDL_GetTicks();

        handleEvents(event);
        handleEventsTime = SDL_GetTicks() - strt;

        strt = SDL_GetTicks();
        update();

        updateTime = SDL_GetTicks() - strt;
        strt = SDL_GetTicks();

        render();
        renderTime = SDL_GetTicks() - strt;

        if (debug) {
            std::cout <<
                std::left << "E: " << std::right << std::setw(4) << std::to_string(handleEventsTime) + "ms" << 
                std::left << std::setw(5) << "" << 
                std::left << "U: " << std::right << std::setw(4) << std::to_string(updateTime) + "ms" << 
                std::left << std::setw(5) << "" << 
                std::left << "R: " << std::right << std::setw(4) << std::to_string(renderTime) + "ms" <<
                std::left << std::setw(5) << "" << 
                std::left << "M: " << std::right << std::setw(4) << std::to_string(timePerFrame) + "ms" <<
            std::endl;
        }

        H2DE_RenderEngine(engine);
        
        frameCounter++;
        frameTime = SDL_GetTicks() - now;
        if (timePerFrame >= frameTime) SDL_Delay(timePerFrame - frameTime);
        if (SDL_GetTicks() - fpsTimer >= 1000) {
            currentFPS = frameCounter / ((SDL_GetTicks() - fpsTimer) / 1000.0f);
            frameCounter = 0;
            fpsTimer = SDL_GetTicks();
        }
    }
}

// EVENTS
void Game::handleEvents(SDL_Event event) {
    static std::vector<KeyEvent> keyDownEvents = {
        { SDLK_ESCAPE, { MAIN_MENU, DEFAULT }, [this]() { quit(); } },
        { SDLK_SPACE, { MAIN_MENU, DEFAULT }, [this]() { setState({ LEVEL_MENU, DEFAULT }); } },
        { SDLK_ESCAPE, { LEVEL_MENU, DEFAULT }, [this]() { setState({ MAIN_MENU, DEFAULT }); } },
        { SDLK_SPACE, { LEVEL_MENU, DEFAULT }, [this]() { openLevel(); closeMenu(); } },
        { SDLK_SPACE, { LEVEL_PLAYING, DEFAULT }, [this]() { level->getPlayer()->setClicking(true); } },
        { SDLK_UP, { LEVEL_PLAYING, DEFAULT }, [this]() { level->getPlayer()->setClicking(true); } },
        { SDLK_ESCAPE, { LEVEL_PLAYING, DEFAULT }, [this]() { level->pause(); } },
        { SDLK_SPACE, { LEVEL_PAUSE, DEFAULT }, [this]() { level->resume(); } },
        { SDLK_ESCAPE, { LEVEL_PAUSE, DEFAULT }, [this]() { closeLevel(); setState({ LEVEL_MENU, DEFAULT }); openMenu(); } },
    };

    static std::vector<KeyEvent> keyUpEvents = {
        { SDLK_SPACE, { LEVEL_PLAYING, DEFAULT }, [this]() { level->getPlayer()->setClicking(false); } },
        { SDLK_UP, { LEVEL_PLAYING, DEFAULT }, [this]() { level->getPlayer()->setClicking(false); } },
    };

    while (SDL_PollEvent(&event)) switch (event.type) {
        case SDL_QUIT: quit(); break;
        case SDL_WINDOWEVENT: switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED: resizeWindow(event.window.data1, event.window.data2); break;
        } break;
        case SDL_MOUSEBUTTONDOWN: if (event.button.button == SDL_BUTTON_LEFT) switch (state.main) {
            case LEVEL_STARTING_DELAY: level->getPlayer()->setClicking(true); break;
            case LEVEL_PLAYING: level->getPlayer()->setClicking(true); break;
            case LEVEL_DEAD: level->getPlayer()->setClicking(true); break;
        } break;
        case SDL_MOUSEBUTTONUP: if (event.button.button == SDL_BUTTON_LEFT) switch (state.main) {
            case LEVEL_STARTING_DELAY: level->getPlayer()->setClicking(false); break;
            case LEVEL_PLAYING: level->getPlayer()->setClicking(false); break;
            case LEVEL_DEAD: level->getPlayer()->setClicking(false); break;
        } break;
        case SDL_KEYDOWN: for (KeyEvent e : keyDownEvents) {
            bool sameKey = (event.key.keysym.sym == e.keycode);
            bool sameState = (state.main == e.state.main && state.sub == e.state.sub);
            if (sameKey && sameState) {
                e.call();
                break;
            }
        } break;
        case SDL_KEYUP: for (KeyEvent e : keyUpEvents) {
            bool sameKey = (event.key.keysym.sym == e.keycode);
            bool sameState = (state.main == e.state.main && state.sub == e.state.sub);
            if (sameKey && sameState) {
                e.call();
                break;
            }
        } break;
    }
}

void Game::resizeWindow(int w, int h) {
    int finalW = w;
    int finalH = w / 16 * 9;

    SDL_SetWindowSize(window, finalW, finalH);
    H2DE_SetEngineSize(engine, finalW, finalH);
}

// OPEN / CLOSE
void Game::openMenu() {
    menu = new Menu(this);
}

void Game::closeMenu() {
    if (menu) {
        delete menu;
        menu = nullptr;
    }
}

void Game::openLevel() {
    int levelID = (menu) ? menu->getLevelIndex() : 0;
    level = new Level(this, levelID);
}

void Game::closeLevel() {
    if (level) {
        delete level;
        level = nullptr;
    }
}

// UPDATE
void Game::update() {
    static std::vector<UpdateInstruction> updateInstructions = {
        { { MAIN_MENU, DEFAULT }, [this]() { menu->update(); } },
        { { LEVEL_PLAYING, DEFAULT }, [this]() { level->update(); } },
    };

    for (UpdateInstruction u : updateInstructions) {
        bool sameState = (state.main == u.state.main && (u.state.sub == DEFAULT || state.sub == u.state.sub));
        if (sameState) u.call();
    }
}

// RENDER
void Game::render() {
    static std::vector<RenderInstruction> renderInstructions = {
        { { MAIN_MENU, DEFAULT }, [this]() { menu->render(); } },
        { { LEVEL_MENU, DEFAULT }, [this]() { menu->render(); } },
        { { LEVEL_STARTING_DELAY, DEFAULT }, [this]() { level->render(); } },
        { { LEVEL_PLAYING, DEFAULT }, [this]() { level->render(); } },
        { { LEVEL_PAUSE, DEFAULT }, [this]() { level->render(); } },
        { { LEVEL_END, DEFAULT }, [this]() { level->render(); } },
    };

    for (RenderInstruction u : renderInstructions) {
        bool sameState = (state.main == u.state.main && (u.state.sub == DEFAULT || state.sub == u.state.sub));
        if (sameState) u.call();
    }
}

// STATIC
void Game::delay(int ms, std::function<void()> callback) {
    std::thread([ms, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        callback();
    }).detach();
}

// GETTER
GameData* Game::getData() {
    return data;
}

H2DE_Engine* Game::getEngine() {
    return engine;
}

Calculator* Game::getCalculator() {
    return calculator;
}

Camera* Game::getCamera() {
    return camera;
}

Level* Game::getLevel() {
    return level;
}

GameState Game::getState() {
    return state;
}

// SETTER
void Game::setState(GameState s) {
    static std::vector<std::string> mains = {
        "loading screen",
        "main menu",
        "level menu",
        "icons menu",
        "level starting delay",
        "level playing",
        "level dead",
        "level pause",
        "level end",
    };
    static std::vector<std::string> subs = {
        "default",
        "taénsition in",
        "taénsition out",
    };
    state.main = s.main;
    state.sub = s.sub;
    std::cout << "Game state: " << mains[state.main] << " + (" << subs[state.sub] << ")" << std::endl;
}