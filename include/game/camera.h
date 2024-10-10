#ifndef CAMERA_H
#define CAMERA_H

#include "game.h"
class Game;

class Camera {
private:
    Game* game;

    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

    LevelPos initalPos;
    LevelPos currentPos;
    Velocity velocity = { 0.0f, 0.0f };

public:
    Camera(Game* game);
    ~Camera();

    void update();
    void reset();
    void clearTM();

    LevelPos getPos();

    void globalSet(LevelPos pos);
    void setPos(LevelPos pos, unsigned int ms);
};

#endif
