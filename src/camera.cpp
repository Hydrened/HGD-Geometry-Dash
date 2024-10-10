#include "camera.h"

// INIT
Camera::Camera(Game* g) : game(g) {
    static GameData* gameData = game->getData();
    
    globalSet(gameData->positions->camera);
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
    float newYpos = currentPos.y;

    if (gameData->physics->canMoveCamera[player->getGamemode()]) {
        if (playerPos.y < currentPos.y + cameraPaddingBot) {
            newYpos -= ((( currentPos.y + cameraPaddingBot ) - playerPos.y ) / cameraPaddingBot) * maxGravityCube;

        } else if (playerPos.y > currentPos.y + winHeight - cameraPaddingTop) {
            newYpos += (( playerPos.y - ( currentPos.y + winHeight - cameraPaddingTop )) / cameraPaddingTop) * maxGravityCube;
        }

        if (newYpos < cameraMinY) newYpos = cameraMinY;
        else if (newYpos > cameraMaxY) newYpos = cameraMaxY;
    }

    setPos({ currentPos.x + gameData->physics->speeds[level->getCurrentSpeed()],newYpos }, 0);
}

// RESET
void Camera::reset() {
    currentPos.x = initalPos.x;
    currentPos.y = initalPos.y;
}

void Camera::clearTM() {
    H2DE_ClearTimelineManager(tm);
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

void Camera::setPos(LevelPos pos, unsigned int ms) {
    static H2DE_Engine* engine = game->getEngine();
    currentPos.x = pos.x;

    if (ms != 0) {
        LevelPos defaultPos = currentPos;
        H2DE_ClearTimelineManager(tm);

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, ms, EASE_IN_OUT, [this, defaultPos, pos](float blend) {
            currentPos.y = defaultPos.y + blend * (pos.y - defaultPos.y);
        }, NULL, 0);
        H2DE_AddTimelineToManager(tm, t);
    } else currentPos.y = pos.y;

    H2DE_TickTimelineManager(tm);
}
