#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"
class Game;

class Player {
private:
    Game* game;
    const Checkpoint* checkpoint;

    H2DE_Object* cube = nullptr;

    H2DE_LevelPos pos = { 0.0f, 0.0f };
    H2DE_LevelVelocity velocity = { 0.0f, 0.0f };
    float rotation = 0.0f;

    PlayerGamemode gamemode = PLAYER_GAMEMODE_CUBE;
    PlayerSize size = PLAYER_SIZE_NORMAL;
    PlayerGravity gravity = PLAYER_GRAVITY_NORMAL;

    bool botOnSolid = true;
    bool topOnSolid = true;
    bool mouseDown = false;

    void initCheckpoint();
    void initIcons();
    void initIconCreateLayers(const std::string& textureName, const std::vector<std::tuple<std::string, H2DE_AbsRect, H2DE_LevelRect, H2DE_ColorRGB>>& layers, std::unordered_map<std::string, H2DE_Surface*>& surfaces) const;
    void initCube();

    void destroyIcons();

    void updateGravity();
    void updateClicks();
    void updateRotation();
    void updateRotationOnSolid();
    void updateRotationInAir();
    void updateRotationClamp();
    void updatePosition();

public:
    Player(Game* game, const Checkpoint* checkpoint);
    ~Player();

    void update();

    void kill();
    void click();

    const H2DE_LevelPos getPos() const;

    void setVelocityX(float vx);
    void setMouseDown(bool state);
};

#endif
