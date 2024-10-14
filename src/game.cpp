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
    loadVolumes();

    calculator = new Calculator(this);
    megahack = new Megahack(this);

    camera = new Camera(this);
    setState({ MAIN_MENU, DEFAULT }, getTransitionDuration(500), [this]() {
        openMenu();
    });
}

void Game::createWindow(SDL_WindowFlags flag) {
    bool fullscreen = (flag == SDL_WINDOW_FULLSCREEN);
    json* settings = H2DE_Json::read("data/settings.json");

    int x = (fullscreen) ? 0 : static_cast<int>((*settings)["window"]["x"]);
    int y = (fullscreen) ? 0 : static_cast<int>((*settings)["window"]["y"]);
    int w = (fullscreen) ? 1920 : static_cast<int>((*settings)["window"]["w"]);
    int h = (fullscreen) ? 1080 : static_cast<int>((*settings)["window"]["h"]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        throw std::runtime_error("HGD-1000: Error creating window => SDL_Init failed: " + std::string(SDL_GetError()));
    }

    window = SDL_CreateWindow("Geometry Dash 1.0 (1.0.18)", x, y, w, h, flag);
    if (!window) {
        SDL_Quit();
        throw std::runtime_error("HGD-1001: Error creating window => SDL_CreateWindow failed: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw std::runtime_error("HGD-1002: Error creating window => SDL_CreateRenderer failed: " + std::string(SDL_GetError()));
    }

    engine = H2DE_CreateEngine(renderer, w, h, FPS);
    if (!engine) {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        throw std::runtime_error("HGD-1003: Error creating window => H2DE_CreateEngine failed");
    }

    SDL_SetWindowMaximumSize(window, ((fullscreen) ? 1920 : 1600), ((fullscreen) ? 1080 : 900));
    H2DE_SetEngineMaximumSize(engine, ((fullscreen) ? 1920 : 1600), ((fullscreen) ? 1080 : 900));
}

void Game::loadVolumes() {
    std::string SAVESpath = "data/saves.json";
    json* saves = H2DE_Json::read(SAVESpath);

    json volume = (*saves)["volume"];
    H2DE_SetSongVolume(engine, volume["song"]);
    H2DE_SetSFXVolume(engine, -1, volume["sfx"]);
}

// CLEANUP
Game::~Game() {
    saveSettings();
    if (menu) delete menu;
    if (level) delete level;

    delete camera;
    delete calculator;
    delete megahack;
    delete data;

    H2DE_DestroyTimelineManager(tm);

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

        if (!H2DE_Json::write(SETTINGSpath, settings, 4)) {
            throw std::runtime_error("HGD-3001: Error updating settings => Writing settings failed");
        }
    }
}

// RUN
void Game::run() {
    Uint32 now, fpsTimer, strt, handleEventsTime, updateTime, renderTime = SDL_GetTicks();
    int frameTime;
    SDL_Event event;

    while (isRunning) {
        float speed = (megahack->getHack("speedhack")->active) ? 0.5f: 1.0f;
        int timePerFrame = 1000 / FPS / speed;

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
        
        frameTime = SDL_GetTicks() - now;
        if (timePerFrame >= frameTime) SDL_Delay(timePerFrame - frameTime);
    }
}

// EVENTS
void Game::handleEvents(SDL_Event event) {
    static std::vector<KeyEvent> keyDownEvents = {
        { SDLK_ESCAPE, { { MAIN_MENU, DEFAULT } }, [this]() {
            quit();
        } },
        { SDLK_ESCAPE, { { LEVEL_MENU, DEFAULT } }, [this]() {
            menu->resetMainMenu();
            setState({ MAIN_MENU, DEFAULT }, getTransitionDuration(500), NULL);
        } },
        { SDLK_ESCAPE, { { LEVEL_PLAYING, DEFAULT } }, [this]() {
            level->pause();
        } },
        { SDLK_ESCAPE, { { LEVEL_PAUSE, DEFAULT }, { LEVEL_END, DEFAULT } }, [this]() {
            setState({ LEVEL_MENU, DEFAULT }, getTransitionDuration(500), [this]() {
                closeLevel();
                openMenu();
            });
        } },
        { SDLK_ESCAPE, { { MAIN_MENU, MODAL_EXIT } }, [this]() {
            closeModal();
        } },



        { SDLK_SPACE, { { MAIN_MENU, DEFAULT } }, [this]() {
            setState({ LEVEL_MENU, DEFAULT }, getTransitionDuration(500), NULL);
        } },
        { SDLK_SPACE, { { LEVEL_MENU, DEFAULT } }, [this]() {
            H2DE_PlaySFX(engine, "play-level.ogg", 0);
            setState({ LEVEL_STARTING_DELAY, DEFAULT }, getTransitionDuration(500), [this]() {
                openLevel();
                closeMenu();
            });
        } },
        { SDLK_SPACE, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT } }, [this]() {
            level->getPlayer()->setClicking(true);
        } },
        { SDLK_SPACE, { { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->resume();
        } },



        { SDLK_UP, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT }, { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->getPlayer()->setClicking(true);
        } },
        { SDLK_LEFT, { { LEVEL_MENU, DEFAULT } }, [this]() {
            menu->incrLevelIndex(-1);
        } },
        { SDLK_RIGHT, { { LEVEL_MENU, DEFAULT } }, [this]() {
            menu->incrLevelIndex(1);
        } },



        { SDLK_r, { { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->respawn();
        } },
        { SDLK_p, { { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->setMode((level->getMode() == NORMAL_MODE) ? PRACTICE_MODE : NORMAL_MODE);
            if (level->getMode() == NORMAL_MODE) level->respawn();
            level->getPlayer()->clearCheckpoints();
        } },
        { SDLK_w, { { LEVEL_PLAYING, DEFAULT } }, [this]() {
            if (level->getMode() == PRACTICE_MODE && this->canAddCheckpoint) {
                level->getPlayer()->addCheckpoint();
                this->canAddCheckpoint = false;
            }
        } },
        { SDLK_x, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT } }, [this]() {
            if (level->getMode() == PRACTICE_MODE && this->canRemoveCheckpoint) {
                level->getPlayer()->removeLastCheckpoint();
                this->canRemoveCheckpoint = false;
            }
        } },
    };

    static std::vector<KeyEvent> keyUpEvents = {
        { SDLK_SPACE, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT }, { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->getPlayer()->setClicking(false);
        } },

        { SDLK_UP, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT }, { LEVEL_PAUSE, DEFAULT } }, [this]() {
            level->getPlayer()->setClicking(false);
        } },

        { SDLK_w, { { LEVEL_PLAYING, DEFAULT } }, [this]() {
            if (level->getMode() == PRACTICE_MODE) this->canAddCheckpoint = true;
        } },
        { SDLK_x, { { LEVEL_PLAYING, DEFAULT }, { LEVEL_DEAD, DEFAULT } }, [this]() {
            if (level->getMode() == PRACTICE_MODE) this->canRemoveCheckpoint = true;
        } },
    };

    while (SDL_PollEvent(&event)) switch (event.type) {
        case SDL_QUIT: quit(); break;

        case SDL_WINDOWEVENT: switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED: resizeWindow(event.window.data1, event.window.data2); break;
        } break;

        case SDL_MOUSEBUTTONDOWN: if (event.button.button == SDL_BUTTON_LEFT) {
            switch (state.main) {
                case LEVEL_STARTING_DELAY: level->getPlayer()->setClicking(true); break;
                case LEVEL_PLAYING: level->getPlayer()->setClicking(true); break;
                case LEVEL_PAUSE: level->getPlayer()->setClicking(true); break;
                case LEVEL_DEAD: level->getPlayer()->setClicking(true); break;
            }
            H2DE_Click(engine, event.button.x, event.button.y);
        } break;

        case SDL_MOUSEBUTTONUP: if (event.button.button == SDL_BUTTON_LEFT) switch (state.main) {
            case LEVEL_STARTING_DELAY: level->getPlayer()->setClicking(false); break;
            case LEVEL_PLAYING: level->getPlayer()->setClicking(false); break;
            case LEVEL_PAUSE: level->getPlayer()->setClicking(false); break;
            case LEVEL_DEAD: level->getPlayer()->setClicking(false); break;
        } break;

        case SDL_KEYDOWN:
            for (const auto& hack : megahack->getHacks()) if (event.key.keysym.sym == hack.second->keycode) {
                hack.second->active = !hack.second->active;
                std::cout << hack.first << " is now " << ((hack.second->active) ? "enabled" : "disabled") << std::endl;
            }

            for (KeyEvent e : keyDownEvents) {
                bool sameKey = (event.key.keysym.sym == e.keycode);
                bool sameState = (std::find(e.states.begin(), e.states.end(), state) != e.states.end());
                if (sameKey && sameState) {
                    e.call();
                    break;
                }
            }
            break;

        case SDL_KEYUP: for (KeyEvent e : keyUpEvents) {
            bool sameKey = (event.key.keysym.sym == e.keycode);
            bool sameState = (std::find(e.states.begin(), e.states.end(), state) != e.states.end());
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

void Game::openModal(GameState state) {
    modal = new Modal(this, state);
}

void Game::closeModal() {
    delete modal;
    modal = nullptr;
    setState({ state.main, DEFAULT });
}

// UPDATE
void Game::update() {
    static std::vector<UpdateInstruction> updateInstructions = {
        { { { MAIN_MENU, DEFAULT }, { LEVEL_MENU, DEFAULT } }, [this]() {
            if (menu) menu->update();
        } },
        { { { LEVEL_PLAYING, DEFAULT } }, [this]() {
            if (level) {
                level->update();
                megahack->update();
            }
        } },
    };

    for (UpdateInstruction u : updateInstructions) {
        bool sameState = false;
        for (GameState us : u.states) {
            if (state.main == us.main && (us.sub == DEFAULT || state.sub == us.sub)) {
                sameState = true;
                break;
            }
        }

        if (sameState) u.call();
    }

    if (modal) modal->update();
}

// RENDER
void Game::render() {
    static std::vector<RenderInstruction> renderInstructions = {
        { { { MAIN_MENU, DEFAULT }, { LEVEL_MENU, DEFAULT } }, [this]() {
            if (menu) menu->render();
        } },
        { { { LEVEL_STARTING_DELAY, DEFAULT }, { LEVEL_PLAYING, DEFAULT }, { LEVEL_PAUSE, DEFAULT }, { LEVEL_END, DEFAULT }, { LEVEL_DEAD, DEFAULT } }, [this]() {
            if (level) {
                level->render();
                megahack->render();
            }
        } },
    };

    for (RenderInstruction r : renderInstructions) {
        bool sameState = false;
        for (GameState us : r.states) {
            if (state.main == us.main && (us.sub == DEFAULT || state.sub == us.sub)) {
                sameState = true;
                break;
            }
        }

        if (sameState) r.call();
    }

    if (modal) modal->render();

    H2DE_TickTimelineManager(tm);

    if (transitionOpacity != 0) {
        H2DE_Size engineSize = H2DE_GetEngineSize(engine);

        H2DE_GraphicObject* transition = H2DE_CreateGraphicObject();
        transition->type = POLYGON;
        transition->pos = { 0, 0 };
        transition->points = {
            { 0, 0 },
            { engineSize.w, 0 },
            { engineSize.w, engineSize.h },
            { 0, engineSize.h },
        };
        transition->rgb = { 0, 0, 0, transitionOpacity };
        transition->filled = true;
        transition->index = 99999;
        H2DE_AddGraphicObject(engine, transition);
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
GameData* Game::getData() const {
    return data;
}

H2DE_Engine* Game::getEngine() const {
    return engine;
}

Calculator* Game::getCalculator() const {
    return calculator;
}

Camera* Game::getCamera() const {
    return camera;
}

Megahack* Game::getMegahack() const {
    return megahack;
}

Level* Game::getLevel() const {
    return level;
}

GameState Game::getState() const {
    return state;
}

unsigned int Game::getTransitionDuration(unsigned int ms) const {
    return (megahack->getHack("no-transition")->active) ? 0 : ms;
}

// SETTER
void Game::setState(GameState s) {
    state.main = s.main;
    state.sub = s.sub;
}

void Game::setState(GameState s, unsigned int ms, std::function<void()> then) {
    if (state.sub == TRANSITION_IN || state.sub == TRANSITION_OUT) return;
    
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
        "transition in",
        "transition out",
        "modal exit",
    };

    state.sub = TRANSITION_IN;

    H2DE_Timeline* in = H2DE_CreateTimeline(engine, (unsigned int)(ms / 2), EASE_IN, [this](float blend) {
        this->transitionOpacity = (Uint8)(blend * 255);

    }, [this, s, ms, then]() {
        state.main = s.main;
        state.sub = TRANSITION_OUT;

        if (then) then();

        H2DE_Timeline* out = H2DE_CreateTimeline(engine, (unsigned int)(ms / 2), EASE_OUT, [this](float blend) {
            this->transitionOpacity = (Uint8)(255 + blend * (0 - 255));

        }, [this, s]() {
            state.sub = s.sub;
            std::cout << "Game state: " << mains[state.main] << " + (" << subs[state.sub] << ")" << std::endl;
        }, 0);

        H2DE_AddTimelineToManager(tm, out);
    }, 0);

    H2DE_AddTimelineToManager(tm, in);
}
