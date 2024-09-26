#include "player.h"

// INIT
Player::Player(Game* g, Checkpoint* s) : game(g), startpos(s), pos(s->playerPos), velocity(s->velocity), gravity(s->gravity), gamemode(s->gamemode), size(s->size), rotation(s->rotation) {
    
}

// CLEANUP
Player::~Player() {

}

// UPDATE
void Player::update() {
    updateClicks();
    updatePositions();
    updateCamera();
    checkGroundCollisions();
    checkItemCollisions();
    updateRotation();
    checkTriggers();
    updatePercentage();
}

void Player::updateClicks() {
    if (!clicking) canBuffer = true;
    if (clicking && canBuffer) click();
}

void Player::updatePositions() {
    static GameData* gameData = game->getData();
    Level* level = game->getLevel();

    onSolid = false;

    velocity.x = gameData->physics->speeds[level->getCurrentSpeed()];
    velocity.y += gameData->physics->gravities[gamemode][size] * gravity;
    pos.x += velocity.x;
    pos.y += velocity.y;

    float maxGravity = gameData->physics->maxGravities[gamemode][size];
    if (velocity.y > maxGravity) velocity.y = maxGravity;
    else if (velocity.y * -1.0f > maxGravity) velocity.y = maxGravity * -1.0f;
}

void Player::updateCamera() {
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();

    H2DE_Size engineSize = H2DE_GetEngineSize(game->getEngine());

    static float winHeight = engineSize.h / round(engineSize.w / BLOCKS_ON_WIDTH);
    static float cameraMinY = gameData->positions->cameraMinY;
    static float cameraMaxY = gameData->positions->cameraMaxY;
    static float cameraPaddingBot = gameData->sizes->cameraPaddingBot;
    static float cameraPaddingTop = gameData->sizes->cameraPaddingTop;
    static float cameraSpeedY = gameData->physics->cameraSpeedY;

    Level* level = game->getLevel();
    LevelPos camPos = camera->getPos();

    float newCamPosY = camPos.y;

    if (gameData->physics->canMoveCamera[gamemode]) {
        if (pos.y < camPos.y + cameraPaddingBot) newCamPosY -= cameraSpeedY;
        else if (pos.y > camPos.y + winHeight - cameraPaddingTop) newCamPosY += cameraSpeedY;
        if (newCamPosY < cameraMinY) newCamPosY = cameraMinY;
        else if (newCamPosY > cameraMaxY) newCamPosY = cameraMaxY;
    }

    camera->setPos({ camPos.x + gameData->physics->speeds[level->getCurrentSpeed()], newCamPosY });
}

void Player::checkGroundCollisions() {
    static GameData* gameData = game->getData();
    static float groundHeight = gameData->sizes->ground.h;
    Level* level = game->getLevel();

    if (velocity.y < 0.0f) {
        float bottomGroundsTop = level->getBotGroundPos().y + groundHeight;

        if (pos.y < bottomGroundsTop) {
            if (gravity == RIGHT_SIDE_UP) onSolid = true;
            velocity.y = 0.0f;
            pos.y = bottomGroundsTop;
        }
    } else if (velocity.y > 0.0f) {
        float topGroundsBottom = level->getTopGroundPos().y;
        
        if (pos.y + gameData->sizes->redHitboxSizes[gamemode][size].h > topGroundsBottom) {
            if (gravity == UPSIDE_DOWN) onSolid = true;
            velocity.y = 0.0f;
            pos.y = topGroundsBottom - gameData->sizes->redHitboxSizes[gamemode][size].h;
        }
    }
}

void Player::checkItemCollisions() {
    GameData* gameData = game->getData();
    Level* level = game->getLevel();
    std::vector<Item*>* items = level->getItems();
    LevelPos camPos = game->getCamera()->getPos();

    Rect redPlayerRect = { pos.x + gameData->offsets->redHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->redHitboxOffsets[gamemode][size].y, gameData->sizes->redHitboxSizes[gamemode][size].w, gameData->sizes->redHitboxSizes[gamemode][size].h };
    Rect bluePlayerRect = { pos.x + gameData->offsets->blueHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->blueHitboxOffsets[gamemode][size].y, gameData->sizes->blueHitboxSizes[gamemode][size].w, gameData->sizes->blueHitboxSizes[gamemode][size].h };
    
    for (int i = startingItem; i < items->size(); i++) {
        Item* item = items->at(i);
        Block* block = ItemManager::castToBlock(item);
        
        if (!block) continue;

        BufferedBlock* blockData = block->getData();
        if (blockData->type != SOLID && blockData->type != SPECIAL && blockData->type != OBSTACLE) continue;

        if (blockData->pos.x + blockData->textureOffset.x + blockData->textureSize.w < camPos.x) {
            startingItem = i + 1;
            continue;
        }
        if (blockData->pos.x + blockData->textureOffset.x + blockData->textureSize.w > camPos.x + BLOCKS_ON_WIDTH + 1) break;

        LevelPos blockPos = blockData->pos;
        LevelSize blockSize = blockData->hitboxSize;
        LevelOffset blockOffset = blockData->hitboxOffset;
        Rect blockRect = (blockData->type != SPECIAL) ?
            Rect{ blockPos.x + blockOffset.x, blockPos.y + blockOffset.y, blockSize.w, blockSize.h } :
            Rect{ blockPos.x + blockOffset.x - 0.01f, blockPos.y + blockOffset.y - 0.01f, blockSize.w + 0.01f, blockSize.h + 0.01f };

        if (Rect::intersect(&redPlayerRect, &blockRect)) {
            bool canHitTop = gameData->physics->canHitTop[gamemode];
            bool canHitBottom = gameData->physics->canHitBottom[gamemode];

            switch (blockData->type) {
                case SOLID: switch (Rect::getCollidedFace(&redPlayerRect, &blockRect)) {
                    case TOP: if ((gravity == RIGHT_SIDE_UP && canHitTop) || (gravity == UPSIDE_DOWN && canHitBottom)) {
                        onSolid = true;
                        velocity.y = 0;
                        pos.y = blockData->pos.y - gameData->sizes->redHitboxSizes[gamemode][size].h;
                    } break;
                    case BOTTOM: if ((gravity == RIGHT_SIDE_UP && canHitBottom) || (gravity == UPSIDE_DOWN && canHitTop)) {
                        onSolid = true;
                        velocity.y = 0;
                        pos.y = blockData->pos.y + blockData->hitboxSize.h + blockData->hitboxOffset.y;
                    } break;
                } break;
                // case SPECIAL: if (!item->entered()) {
                //     item->enter();
                //     switch (itemData->specialData.type) {
                //         case COIN: break;
                //         case PORTAL: 
                //             if (itemData->specialData.gamemode.has_value()) {
                //                 setGamemode(itemData->specialData.gamemode.value(), itemPos.y);
                //             }
                //             break;
                //         case ORB: break;
                //         case PAD: break;
                //     }
                // } break;
            }
        }
    }
}

void Player::updateRotation() {
    static GameData* gameData = game->getData();
    float rotationIncr = gameData->physics->rotations[gamemode][size];

    switch (gamemode) {
        case CUBE:
            if (onSolid) {
                int remain = (gravity == 1) ? 90 - fmod(rotation, 90) : fmod(rotation, 90);
                if (remain != 0) {
                    if (remain * gravity < 45) rotation += (remain * gravity > rotationIncr) ? rotationIncr : remain * gravity;
                    else rotation -= (90 - remain * gravity > rotationIncr) ? rotationIncr : 90 - remain * gravity;
                }
            } else rotation += rotationIncr * gravity;
            break;
        case SHIP:
            rotation = velocity.y * rotationIncr;
            break;
    }

    if (rotation > 360) rotation -= 360;
    else if (rotation < -360) rotation += 360;
}

void Player::checkTriggers() {

}

void Player::updatePercentage() {
    Level* level = game->getLevel();
    
    percentage = pos.x / level->getLevelLength() * 100;
    if (percentage > 100.0) level->finish();
}

// RENDER
void Player::render() {
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();

    LevelPos offsetIconPos = { pos.x + gameData->offsets->iconOffsets[gamemode][size].x, pos.y + gameData->offsets->iconOffsets[gamemode][size].y };
    H2DE_Size absRedHitboxSize = calculator->convertToPx(gameData->sizes->redHitboxSizes[gamemode][size]);
    if (game->getState().main != LEVEL_DEAD) {
        H2DE_GraphicObject* icon = new H2DE_GraphicObject();
        icon->type = IMAGE;
        icon->pos = calculator->convertToPx(offsetIconPos, gameData->sizes->iconSizes[gamemode][size], false, false);
        icon->size = calculator->convertToPx(gameData->sizes->iconSizes[gamemode][size]);
        icon->texture = "cube_59.png"; // replace => 59(player's cube ID)
        icon->rotation = rotation;
        icon->rotationOrigin = { absRedHitboxSize.w / 2, absRedHitboxSize.h / 2 };
        icon->index = Zindex{ T1, 0 }.getIndex();
        H2DE_AddGraphicObject(engine, icon);
    }

    LevelPos offsetRedHitboxPos = { pos.x + gameData->offsets->redHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->redHitboxOffsets[gamemode][size].y };
    H2DE_GraphicObject* redHitbox = new H2DE_GraphicObject();
    redHitbox->type = POLYGON;
    redHitbox->pos = calculator->convertToPx(offsetRedHitboxPos, gameData->sizes->iconSizes[gamemode][size], false, false);
    redHitbox->points = {
        { 0, 0 },
        { absRedHitboxSize.w, 0 },
        { absRedHitboxSize.w, absRedHitboxSize.h },
        { 0, absRedHitboxSize.h },
    };
    redHitbox->color = static_cast<H2DE_Color>(gameData->colors->hitboxes[OBSTACLE]);
    redHitbox->index = Zindex{ H, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, redHitbox);

    LevelPos offsetBlueHitboxPos = { pos.x + gameData->offsets->blueHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->blueHitboxOffsets[gamemode][size].y };
    H2DE_Size blueHitboxSize = calculator->convertToPx(gameData->sizes->blueHitboxSizes[gamemode][size]);
    H2DE_GraphicObject* blueHitbox = new H2DE_GraphicObject();
    blueHitbox->type = POLYGON;
    blueHitbox->pos = calculator->convertToPx(offsetBlueHitboxPos, gameData->sizes->iconSizes[gamemode][size], false, false);
    blueHitbox->points = {
        { 0, 0 },
        { blueHitboxSize.w, 0 },
        { blueHitboxSize.w, blueHitboxSize.h },
        { 0, blueHitboxSize.h },
    };
    blueHitbox->color = static_cast<H2DE_Color>(gameData->colors->hitboxes[SOLID]);
    blueHitbox->index = Zindex{ H, 1 }.getIndex();
    H2DE_AddGraphicObject(engine, blueHitbox);
}

// EVENTS
void Player::click() {
    static GameData* gameData = game->getData();

    switch (gamemode) {
        case CUBE: if (onSolid) {
            canBuffer = true;
            velocity.y = gameData->physics->clicks[CUBE][size] * gravity;
            jumps++;
        } break;
        case SHIP: velocity.y += (gameData->physics->clicks[SHIP][size] * gravity); break;
    }
}

void Player::kill() {
    static H2DE_Engine* engine = game->getEngine();

    clicking = false;
    game->setState({ LEVEL_DEAD, DEFAULT });
    H2DE_PlaySFX(engine, "death-sound.wav", 0);
    H2DE_PauseSong(engine);

    Game::delay(1000, [this]() {

    });
}

// SETTER
void Player::setClicking(bool value) {
    clicking = value;
}
