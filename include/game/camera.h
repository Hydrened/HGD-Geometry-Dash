#ifndef CAMERA_H
#define CAMERA_H

#include "game.h"
class Game;

class Camera {
private:
    Game* game;

    LevelPos initalPos;
    LevelPos currentPos;

public:
    Camera(Game* game);
    ~Camera();

    void reset();
    void globalSet(LevelPos pos);
    void setPos(LevelPos pos);
    LevelPos getPos();
};

#endif
