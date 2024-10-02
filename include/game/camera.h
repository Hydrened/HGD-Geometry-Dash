#ifndef CAMERA_H
#define CAMERA_H

#include "game.h"
class Game;

class Camera {
private:
    Game* game;

    H2DE_TimelineManager* tm = new H2DE_TimelineManager();

    LevelPos initalPos;
    LevelPos currentPos;
    Velocity velocity = { 0.0f, 0.0f };

public:
    Camera(Game* game);
    ~Camera();

    void update();
    void reset();

    LevelPos getPos();

    void globalSet(LevelPos pos);
    void setPos(LevelPos pos, unsigned int ms);
};

#endif
