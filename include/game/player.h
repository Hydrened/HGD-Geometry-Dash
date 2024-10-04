#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"

class Level;
class Block;

class Player {
private:
    Game* game;
    Level* level;

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

    bool botOnSolid = false;
    bool topOnSolid = false;
    bool clicking = false;
    bool clickInit = false;
    bool orbBuffer = false;

    std::optional<Block*> hoveredOrb;

    void click();
    void kill();

    void updatePositions();
    void checkGroundCollisions();
    void checkBlocksCollisions();
    void updateRotation();
    void updatePercentage();
    void updateClicks();

    void renderTexture();
    void renderHitboxes();

public:
    Player(Game* game, Level* level, Checkpoint* startpos);
    ~Player();

    void update();
    void render();
    void reset(Checkpoint* checkpoint);

    LevelPos getPos() const;
    int getJumps() const;
    int getClicks() const;
    Gamemode getGamemode() const;
    Size getSize() const;
    Gravity getGravity() const;
    
    void setClicking(bool value);
    void setGamemode(Gamemode gamemode, float y, unsigned int ms);
    void setGravity(Gravity gravity);
    void setYvelocity(float yv);
    void setHoveredOrb(Block* block);
};

#endif 
