#ifndef CAMERA_H
#define CAMERA_H

#include "game.h"
class Game;

class Camera {
private:
    Game* game;

    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

    LevelPos pos;
    Velocity velocity = { 0.0f, 0.0f };

public:
    Camera(Game* game);
    ~Camera();

    void update();
    void reset(LevelPos pos);
    void clearTM();

    LevelPos getPos();
    bool isOnScreen(LevelPos pos, LevelSize size, LevelOffset offset);

    void setPos(LevelPos pos, unsigned int ms);
};

#endif
