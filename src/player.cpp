#include "player.h"

// INIT
Player::Player(Game* g, Level* l, Checkpoint* s) : game(g), level(l), startpos(s), pos(s->playerPos), velocity(s->velocity), gamemode(s->gamemode), gravity(s->gravity), size(s->size), rotation(s->rotation) {
    setGamemode(s->gamemode, pos.y, 0);
}

// CLEANUP
Player::~Player() {

}

// UPDATE
void Player::update() {
    updateClicks();
    updatePositions();
    checkGroundCollisions();
    checkBlocksCollisions();
    updateRotation();
    updatePercentage();
}

void Player::updateClicks() {
    if (!clicking) canBuffer = true;
    if (clicking && canBuffer) click();
}

void Player::updatePositions() {
    static GameData* gameData = game->getData();

    botOnSolid = false;
    topOnSolid = false;

    velocity.x = gameData->physics->speeds[level->getCurrentSpeed()];
    velocity.y += gameData->physics->gravities[gamemode][size] * gravity;
    pos.x += velocity.x;
    pos.y += velocity.y;

    if (pos.y < -1.0f || pos.y > gameData->sizes->levelHeight) kill();

    float maxGravity = gameData->physics->maxGravities[gamemode][size];
    if (velocity.y > maxGravity) velocity.y = maxGravity;
    else if (velocity.y * -1.0f > maxGravity) velocity.y = maxGravity * -1.0f;
}

void Player::checkGroundCollisions() {
    static GameData* gameData = game->getData();
    static float groundHeight = gameData->sizes->ground.h;

    if (velocity.y < 0.0f) {
        float bottomGroundsTop = level->getBotGroundPos().y + groundHeight;

        if (pos.y < bottomGroundsTop) {
            if (gravity == RIGHT_SIDE_UP) botOnSolid = true;
            else topOnSolid = true;
            velocity.y = 0.0f;
            pos.y = bottomGroundsTop;
        }
    } else if (velocity.y > 0.0f) {
        float topGroundsBottom = level->getTopGroundPos().y;
        
        if (pos.y + gameData->sizes->redHitboxSizes[gamemode][size].h > topGroundsBottom) {
            if (gravity == UPSIDE_DOWN) botOnSolid = true;
            else topOnSolid = true;
            velocity.y = 0.0f;
            pos.y = topGroundsBottom - gameData->sizes->redHitboxSizes[gamemode][size].h;
        }
    }
}

void Player::checkBlocksCollisions() {
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();
    static std::unordered_map<std::string, Hack*> hacks = game->getMegahack()->getHacks();
    std::vector<Item*>* items = level->getItems();
    LevelPos camPos = camera->getPos();

    Rect redPlayerRect = { pos.x + gameData->offsets->redHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->redHitboxOffsets[gamemode][size].y, gameData->sizes->redHitboxSizes[gamemode][size].w, gameData->sizes->redHitboxSizes[gamemode][size].h };
    Rect bluePlayerRect = { pos.x + gameData->offsets->blueHitboxOffsets[gamemode][size].x, pos.y - gameData->offsets->blueHitboxOffsets[gamemode][size].y, gameData->sizes->blueHitboxSizes[gamemode][size].w, gameData->sizes->blueHitboxSizes[gamemode][size].h };
    
    for (int i = startingItem; i < items->size(); i++) {
        Item* item = items->at(i);
        Block* block = ItemManager::castToBlock(item);
        
        if (!block) continue;

        BufferedBlock* blockData = block->getData();
        if (blockData->type == DECORATION) continue;

        if (blockData->pos.x + blockData->textureOffset.x + blockData->textureSize.w < camPos.x) {
            startingItem = i + 1;
            continue;
        }
        if (blockData->pos.x + blockData->textureOffset.x + blockData->textureSize.w > camPos.x + BLOCKS_ON_WIDTH + 1) break;

        LevelPos blockPos = blockData->pos;
        LevelSize blockSize = blockData->hitboxSize;
        LevelOffset blockOffset = blockData->hitboxOffset;
        Rect blockRect = (blockData->type == SPECIAL) ?
            Rect{ blockPos.x + blockOffset.x - 0.01f, blockPos.y + blockOffset.y - 0.01f, blockSize.w + 0.01f, blockSize.h + 0.01f } :
            Rect{ blockPos.x + blockOffset.x, blockPos.y + blockOffset.y, blockSize.w, blockSize.h };

        if (blockData->type != OBSTACLE) {
            if (Rect::intersect(&redPlayerRect, &blockRect)) {
                bool canHitTop = gameData->physics->canHitTop[gamemode];
                bool canHitBottom = gameData->physics->canHitBottom[gamemode];

                switch (blockData->type) {
                    case SOLID: switch (Rect::getCollidedFace(&redPlayerRect, &blockRect)) {
                        case TOP: if ((gravity == RIGHT_SIDE_UP && canHitTop) || (gravity == UPSIDE_DOWN && canHitBottom)) {
                            botOnSolid = (gravity == UPSIDE_DOWN);
                            topOnSolid = !botOnSolid;
                            velocity.y = 0;
                            pos.y = blockData->pos.y - gameData->sizes->redHitboxSizes[gamemode][size].h + blockData->hitboxOffset.y;
                        } break;

                        case BOTTOM: if ((gravity == RIGHT_SIDE_UP && canHitBottom) || (gravity == UPSIDE_DOWN && canHitTop)) {
                            botOnSolid = (gravity == RIGHT_SIDE_UP);
                            topOnSolid = !botOnSolid;
                            velocity.y = 0;
                            pos.y = blockData->pos.y + blockData->hitboxSize.h + blockData->hitboxOffset.y;
                        } break;
                    } break;

                    case SPECIAL: if (!block->entered()) {
                        block->enter();
                    } break;

                    redPlayerRect = { pos.x + gameData->offsets->redHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->redHitboxOffsets[gamemode][size].y, gameData->sizes->redHitboxSizes[gamemode][size].w, gameData->sizes->redHitboxSizes[gamemode][size].h };
                    bluePlayerRect = { pos.x + gameData->offsets->blueHitboxOffsets[gamemode][size].x, pos.y - gameData->offsets->blueHitboxOffsets[gamemode][size].y, gameData->sizes->blueHitboxSizes[gamemode][size].w, gameData->sizes->blueHitboxSizes[gamemode][size].h };
                }
            }
        }

        if (!hacks["noclip"]->active) {
            if (blockData->type != SPECIAL) {
                Rect playerRect = (blockData->type == SOLID) ? bluePlayerRect : redPlayerRect;

                if (Rect::intersect(&playerRect, &blockRect)) {
                    kill();

                    float overflow;
                    switch (Rect::getCollidedFace(&playerRect, &blockRect)) {
                        case TOP:
                            overflow = playerRect.getMaxY() - blockRect.getMinY();
                            pos.y -= overflow;
                            break;

                        case RIGHT:
                            overflow = playerRect.getMaxX() - blockRect.getMinX();
                            camera->setPos({ camPos.x - overflow + velocity.x - gameData->physics->speeds[level->getCurrentSpeed()], camPos.y }, 0);
                            pos.x -= overflow;
                            break;

                        case BOTTOM:
                            overflow = blockRect.getMaxY() - playerRect.getMinY();
                            pos.y += overflow;
                            break;
                    }
                    break;
                }
            }
        }
    }
}

void Player::updateRotation() {
    static GameData* gameData = game->getData();
    float gmRotation = gameData->physics->rotations[gamemode][size];
    float defaultRotationIncr = gameData->physics->rotations[CUBE][size];

    if (botOnSolid || topOnSolid) {
        int remain = (gravity == RIGHT_SIDE_UP) ? 90 - fmod(std::abs(rotation), 90) : fmod(std::abs(rotation), 90);
        int rotationSense = (botOnSolid) ? 1 : -1;

        if (remain != 0) {
            if (remain < 45) rotation += ((remain > defaultRotationIncr) ? defaultRotationIncr : remain) * rotationSense;
            else rotation -= ((90 - remain > defaultRotationIncr) ? defaultRotationIncr : 90 - remain) * rotationSense;
        }
    } else switch (gamemode) {
        case CUBE: rotation += gmRotation * gravity; break;
        case SHIP: rotation = velocity.y * gmRotation; break;
    }

    if (rotation > 360) rotation -= 360;
    else if (rotation < -360) rotation += 360;
}

void Player::updatePercentage() {
    percentage = pos.x / level->getLevelLength() * 100;
    if (percentage > 100.0) level->finish();
}

// RENDER
void Player::render() {
    renderTexture();
    if (game->getMegahack()->getHack("show-hitboxes")->active) renderHitboxes();
}

void Player::renderTexture() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    H2DE_Size absRedHitboxSize = calculator->convertToPx(gameData->sizes->redHitboxSizes[gamemode][size]);
    LevelPos offsetIconPos = { pos.x + gameData->offsets->iconOffsets[gamemode][size].x, pos.y + gameData->offsets->iconOffsets[gamemode][size].y };
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
}

void Player::renderHitboxes() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    H2DE_Size absRedHitboxSize = calculator->convertToPx(gameData->sizes->redHitboxSizes[gamemode][size]);
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
        case CUBE: if (botOnSolid) {
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
    H2DE_PauseSong(engine);
    H2DE_PlaySFX(engine, "death-sound.wav", 0);

    Game::delay(1000, [this]() {
        level->respawn();
    });
}

void Player::reset(Checkpoint* c) {
    pos.x = c->playerPos.x;
    pos.y = c->playerPos.y;

    velocity.x = c->velocity.x;
    velocity.y = c->velocity.y;

    gravity = c->gravity;
    size = c->size;
    setGamemode(c->gamemode, pos.y, 0);

    rotation = 0;
    percentage = 0;

    botOnSolid = false;
    topOnSolid = false;
    startingItem = 0;
}

// GETTER
LevelPos Player::getPos() const {
    return pos;
}

int Player::getClicks() const {
    return clicks;
}

int Player::getJumps() const {
    return jumps;
}

Gamemode Player::getGamemode() const {
    return gamemode;
}

// SETTER
void Player::setClicking(bool value) {
    clicking = value;
    if (!value) clicks++;
}

void Player::setGamemode(Gamemode g, float y, unsigned int ms) {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();
    H2DE_Size winSize = H2DE_GetEngineSize(engine);
    LevelPos topGroundPos = level->getTopGroundPos();
    LevelPos botGroundPos = level->getBotGroundPos();
    LevelPos camPos = camera->getPos();

    gamemode = g;
    int gamemodeHeight = gameData->sizes->gamemodeHeights[g];

    float newTopGroundPosY, newBotGroundPosY;
    if (gamemodeHeight != -1) {
        if (y < 5.0f) y = 5.0f;

        float winHeight = winSize.h / round(winSize.w / BLOCKS_ON_WIDTH);
        newTopGroundPosY = floor((gamemodeHeight - 1.0f) / 2.0f) + y + 1.0f;
        newBotGroundPosY = newTopGroundPosY - gamemodeHeight;

        camera->setPos({ camPos.x, newBotGroundPosY - (winHeight - gamemodeHeight) / 2 }, ms);
    } else {
        newTopGroundPosY = gameData->positions->topGroundPos.y;
        newBotGroundPosY = gameData->positions->botGroundPos.y + gameData->sizes->ground.h;
    }

    level->setTopGroundPos({ topGroundPos.x, newTopGroundPosY }, ms);
    level->setBotGroundPos({ botGroundPos.x, newBotGroundPosY - gameData->sizes->ground.h }, ms);
}

void Player::setGravity(Gravity g) {
    gravity = g;
}
