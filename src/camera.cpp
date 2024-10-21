#include "camera.h"

// INIT
Camera::Camera(Game* g) : game(g) {
    static GameData* gameData = game->getData();
    
    setPos(gameData->positions->camera, 0);
}

// CLEANUP
Camera::~Camera() {
    H2DE_DestroyTimelineManager(tm);
}

// EACH FRAME
void Camera::update() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    
    Level* level = game->getLevel();
    Player* player = level->getPlayer();
    H2DE_Size engineSize = H2DE_GetEngineSize(engine);
    float winHeight = engineSize.h / round(engineSize.w / BLOCKS_ON_WIDTH);

    static float cameraMinY = gameData->positions->cameraMinY;
    static float cameraMaxY = gameData->positions->cameraMaxY;
    static float cameraPaddingBot = gameData->sizes->cameraPaddingBot;
    static float cameraPaddingTop = gameData->sizes->cameraPaddingTop;
    static float maxGravityCube = gameData->physics->maxGravities[CUBE][BIG];

    LevelPos playerPos = player->getPos();
    float newYpos = pos.y;

    if (gameData->physics->canMoveCamera[player->getGamemode()]) {
        if (playerPos.y < pos.y + cameraPaddingBot) {
            newYpos -= ((( pos.y + cameraPaddingBot ) - playerPos.y ) / cameraPaddingBot) * maxGravityCube;

        } else if (playerPos.y > pos.y + winHeight - cameraPaddingTop) {
            newYpos += (( playerPos.y - ( pos.y + winHeight - cameraPaddingTop )) / cameraPaddingTop) * maxGravityCube;
        }

        if (newYpos < cameraMinY) newYpos = cameraMinY;
        else if (newYpos > cameraMaxY) newYpos = cameraMaxY;
    }

    setPos({ pos.x + gameData->physics->speeds[level->getCurrentSpeed()],newYpos }, 0);
}

// RESET
void Camera::reset(LevelPos p) {
    pos.x = p.x;
    pos.y = p.y;
}

void Camera::clearTM() {
    H2DE_ClearTimelineManager(tm);
}

// GETTER
LevelPos Camera::getPos() {
    return pos;
}

bool Camera::isOnScreen(LevelPos p, LevelSize s, LevelOffset o) {
    static float camWidth = game->getData()->sizes->camera.w; 
    return (p.x + o.x + s.w >= pos.x) && (p.x + o.x <= pos.x + camWidth);
}

//SETTER
void Camera::setPos(LevelPos p, unsigned int ms) {
    static H2DE_Engine* engine = game->getEngine();
    pos.x = p.x;

    if (ms != 0) {
        LevelPos defaultPos = pos;
        H2DE_ClearTimelineManager(tm);

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, ms, EASE_IN_OUT, [this, defaultPos, p](float blend) {
            pos.y = lerp(defaultPos.y, p.y, blend);
        }, NULL, 0);
        H2DE_AddTimelineToManager(tm, t);
    } else pos.y = p.y;

    H2DE_TickTimelineManager(tm);
}
