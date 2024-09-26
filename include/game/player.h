#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

class Player {
private:
    Game* game;

    Checkpoint* startpos;
    int jumps;
    int clicks;
    
    LevelPos pos;
    Velocity velocity;
    Gravity gravity;
    Gamemode gamemode;
    Size size;
    int rotation;

    float percentage = 0.0f;
    int startingItem = 0;

    bool clicking = false;
    bool onSolid = false;
    bool canBuffer = true; // retirer et get depuis le GameData

    void click();
    void kill();

    void updateClicks();
    void updatePositions();
    void updateCamera();
    void checkGroundCollisions();
    void checkItemCollisions();
    void updateRotation();
    void checkTriggers();
    void updatePercentage();

public:
    Player(Game* game, Checkpoint* startpos);
    ~Player();

    void update();
    void render();

    void setClicking(bool value);
};

#endif 
