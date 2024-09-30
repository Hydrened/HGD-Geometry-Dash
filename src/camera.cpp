#include "camera.h"

// INIT
Camera::Camera(Game* g) : game(g) {

}

// CLEANUP
Camera::~Camera() {
    H2DE_ClearTimelineManager(tm);
    delete tm;
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

void Camera::setPos(LevelPos pos, int ms) {
    static H2DE_Engine* engine = game->getEngine();
    currentPos.x = pos.x;

    if (ms != 0) {
        LevelPos defaultPos = currentPos;
        H2DE_ClearTimelineManager(tm);

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, ms, EASE_IN_OUT, [this, defaultPos, pos](float blend) {
            currentPos.y = defaultPos.y + blend * (pos.y - defaultPos.y);
        }, NULL);
        H2DE_AddTimelineToManager(tm, t);
    } else currentPos.y = pos.y;

    H2DE_TickTimelineManager(tm);
}
