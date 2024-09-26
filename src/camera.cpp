#include "camera.h"

// INIT
Camera::Camera(Game* g) : game(g) {

}

// CLEANUP
Camera::~Camera() {

}

// RESET
void Camera::reset() {
    currentPos.x = initalPos.x;
    currentPos.y = initalPos.y;
}

// GETTER
LevelPos Camera::getPos() {
    return currentPos;
}

//SETTER
void Camera::globalSet(LevelPos pos) {
    initalPos.x = pos.x;
    initalPos.y = pos.y;
    reset();
}

void Camera::setPos(LevelPos pos) {
    currentPos.x = pos.x;
    currentPos.y = pos.y;
}
