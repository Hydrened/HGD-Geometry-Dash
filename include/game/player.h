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
    bool canBuffer = true;

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

    void renderTexture();
    void renderHitboxes();

public:
    Player(Game* game, Checkpoint* startpos);
    ~Player();

    void update();
    void render();

    LevelPos getPos();
    void setClicking(bool value);
};

#endif 
