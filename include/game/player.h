#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

class Player {
private:
    Game* game;

    Checkpoint* startpos;
    int jumps = 0;
    int clicks = 0;
    
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
    void updatePercentage();

    void renderTexture();
    void renderHitboxes();

public:
    Player(Game* game, Checkpoint* startpos);
    ~Player();

    void update();
    void render();
    void reset(Checkpoint* checkpoint);

    LevelPos getPos();
    int getJumps();
    int getClicks();
    void setClicking(bool value);
    void setGamemode(Gamemode gamemode, float y);
};

#endif 
