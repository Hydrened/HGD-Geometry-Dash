#ifndef GAME_H
#define GAME_H

#include <H2DE/H2DE.h>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>
#include "utils.h"
#include "camera.h"
#include "megahack.h"
#include "menu.h"
#include "level_loader.h"
#include "level.h"
#include "data.h"
#include "calculator.h"
class Camera;
class Megahack;
class Menu;
class Level;
class Calculator;

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    H2DE_Engine* engine;
    int FPS;
    bool isRunning = true;
    bool debug = false;
    GameData* data = new GameData();
    Calculator* calculator;
    H2DE_TimelineManager* tm = new H2DE_TimelineManager();

    Camera* camera = nullptr;
    Megahack* megahack = nullptr;
    Menu* menu = nullptr;
    Level* level = nullptr;
    GameState state;

    Uint8 transitionOpacity = 0;
    bool canAddCheckpoint = true;
    bool canRemoveCheckpoint = true;

    void createWindow(SDL_WindowFlags flag);
    void loadVolumes();
    void saveSettings();
    void resizeWindow(int w, int h);

    void openMenu();
    void closeMenu();
    void openLevel();
    void closeLevel();

public:
    Game(int fps, int argc, char** argv);
    ~Game();

    void run();
    void quit();
    void handleEvents(SDL_Event event);
    void update();
    void render();

    static void delay(int ms, std::function<void()> callback);

    GameData* getData() const;
    H2DE_Engine* getEngine() const;
    Calculator* getCalculator() const;
    Camera* getCamera() const;
    Megahack* getMegahack() const;
    Level* getLevel() const;
    GameState getState() const;
    unsigned int getTransitionDuration(unsigned int ms) const;

    void setState(GameState state, unsigned int ms, std::function<void()> then);
};

#endif
