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

public:
    Camera(Game* game);
    ~Camera();

    void reset();
    void globalSet(LevelPos pos);
    void setPos(LevelPos pos, int ms);
    LevelPos getPos();
};

#endif
