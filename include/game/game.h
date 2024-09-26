#ifndef GAME_H
#define GAME_H

#include <H2DE/H2DE.h>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "utils.h"
#include "camera.h"
#include "megahack.h"
#include "menu.h"
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

    Camera* camera = nullptr;
    Megahack* megahack = nullptr;
    Menu* menu = nullptr;
    Level* level = nullptr;
    GameState state;

    void createWindow(SDL_WindowFlags flag);
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

    GameData* getData();
    H2DE_Engine* getEngine();
    Calculator* getCalculator();
    Camera* getCamera();
    Level* getLevel();
    void setState(GameState state);
    GameState getState();
};

#endif
