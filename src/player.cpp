#include "player.h"

// INIT
Player::Player(Game* g, Level* l, Checkpoint* s) : game(g), level(l), startpos(s), pos(s->playerPos), velocity(s->velocity), gamemode(s->gamemode), gravity(s->gravity), size(s->size), rotation(s->rotation) {
    json* saves = H2DE_Json::read("data/saves.json");

    icons = Icons();
    icons.ids[CUBE] = (*saves)["icons"]["textures"]["cube"];
    icons.ids[SHIP] = (*saves)["icons"]["textures"]["ship"];
    icons.colorIDs.push_back((*saves)["icons"]["colors"][0]);
    icons.colorIDs.push_back((*saves)["icons"]["colors"][1]);
    icons.glow = (*saves)["icons"]["glow"];
    
    setGamemode(s->gamemode, pos.y, 0);
}

// CLEANUP
Player::~Player() {

}

// UPDATE
void Player::update() {
    updatePositions();
    checkGroundCollisions();
    checkBlocksCollisions();
    updateRotation();
    updatePercentage();
    updateClicks();
}

void Player::updatePositions() {
    static GameData* gameData = game->getData();
    static std::unordered_map<std::string, Hack*> hacks = game->getMegahack()->getHacks();

    botOnSolid = false;
    topOnSolid = false;

    velocity.x = gameData->physics->speeds[level->getCurrentSpeed()];
    velocity.y += gameData->physics->gravities[gamemode][size] * gravity;
    pos.x += velocity.x;
    pos.y += velocity.y;

    if (!hacks["noclip"]->active) if (pos.y < -1.0f || pos.y > gameData->sizes->levelHeight) kill();

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
        
        if (pos.y + gameData->sizes->redHitbox[gamemode][size].h > topGroundsBottom) {
            if (gravity == UPSIDE_DOWN) botOnSolid = true;
            else topOnSolid = true;
            velocity.y = 0.0f;
            pos.y = topGroundsBottom - gameData->sizes->redHitbox[gamemode][size].h;
        }
    }
}

void Player::checkBlocksCollisions() {
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();
    static std::unordered_map<std::string, Hack*> hacks = game->getMegahack()->getHacks();
    std::vector<Item*>* items = level->getItems();
    LevelPos camPos = camera->getPos();

    Rect redPlayerRect = { pos.x + gameData->offsets->redHitbox[gamemode][size].x, pos.y + gameData->offsets->redHitbox[gamemode][size].y, gameData->sizes->redHitbox[gamemode][size].w, gameData->sizes->redHitbox[gamemode][size].h };
    Rect bluePlayerRect = { pos.x + gameData->offsets->blueHitbox[gamemode][size].x, pos.y + gameData->offsets->blueHitbox[gamemode][size].y, gameData->sizes->blueHitbox[gamemode][size].w, gameData->sizes->blueHitbox[gamemode][size].h };
    
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

        bool positionChanged = false;

        if (blockData->type != OBSTACLE) {
            if (Rect::intersect(&redPlayerRect, &blockRect)) {
                bool canHitTop = gameData->physics->canHitTop[gamemode];
                bool canHitBottom = gameData->physics->canHitBottom[gamemode];

                Face collidedFace = Rect::getCollidedFace(&redPlayerRect, &blockRect);
                if (collidedBlockFace.find(i) == collidedBlockFace.end()) collidedBlockFace[i] = collidedFace;

                switch (blockData->type) {
                    case SOLID: switch (collidedBlockFace[i]) {
                        case TOP: if ((gravity == RIGHT_SIDE_UP && canHitTop) || (gravity == UPSIDE_DOWN && canHitBottom)) {
                            botOnSolid = (gravity == UPSIDE_DOWN);
                            topOnSolid = !botOnSolid;
                            if (velocity.y > 0) velocity.y = 0;
                            pos.y = blockData->pos.y - gameData->sizes->redHitbox[gamemode][size].h + blockData->hitboxOffset.y;
                            positionChanged = true;
                        } break;

                        case BOTTOM: if ((gravity == RIGHT_SIDE_UP && canHitBottom) || (gravity == UPSIDE_DOWN && canHitTop)) {
                            botOnSolid = (gravity == RIGHT_SIDE_UP);
                            topOnSolid = !botOnSolid;
                            if (velocity.y < 0) velocity.y = 0;
                            pos.y = blockData->pos.y + blockData->hitboxSize.h + blockData->hitboxOffset.y;
                            positionChanged = true;
                        } break;
                    } break;

                    case SPECIAL: if (!block->entered()) {
                        block->enter();
                    } break;
                }

                if (positionChanged) {
                    redPlayerRect = { pos.x + gameData->offsets->redHitbox[gamemode][size].x, pos.y + gameData->offsets->redHitbox[gamemode][size].y, gameData->sizes->redHitbox[gamemode][size].w, gameData->sizes->redHitbox[gamemode][size].h };
                    bluePlayerRect = { pos.x + gameData->offsets->blueHitbox[gamemode][size].x, pos.y + gameData->offsets->blueHitbox[gamemode][size].y, gameData->sizes->blueHitbox[gamemode][size].w, gameData->sizes->blueHitbox[gamemode][size].h };
                }
            } else if (collidedBlockFace.find(i) != collidedBlockFace.end()) collidedBlockFace.erase(i);
        }

        if (!hacks["noclip"]->active && !positionChanged) {
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

    if (botOnSolid || topOnSolid) {
        float defaultRotationIncr = gameData->physics->rotations[CUBE][size];

        int remain = (gravity == RIGHT_SIDE_UP) ? 90 - fmod(std::abs(rotation), 90) : fmod(std::abs(rotation), 90);
        int rotationSense = (botOnSolid) ? 1 : -1;

        if (remain != 0) {
            if (remain < 45) rotation += ((remain > defaultRotationIncr) ? defaultRotationIncr : remain) * rotationSense;
            else rotation -= ((90 - remain > defaultRotationIncr) ? defaultRotationIncr : 90 - remain) * rotationSense;
        }
    } else {
        float maxGravity = gameData->physics->maxGravities[gamemode][size];
        float gmRotation = gameData->physics->rotations[gamemode][size];

        switch (gamemode) {
            case CUBE: rotation += gmRotation * gravity; break;
            case SHIP:
                float a = velocity.y / maxGravity;
                rotation = pow(abs(a), 1.15f) * (a < 0 ? -1 : 1) * gmRotation;
                break;
        }
    }

    if (rotation > 360) rotation -= 360;
    else if (rotation < -360) rotation += 360;
}

void Player::updatePercentage() {
    percentage = pos.x / level->getLevelLength() * 100;
    if (percentage > 100.0) level->finish();
}

void Player::updateClicks() {
    if (clicking) {
        if (gamemode == CUBE && !botOnSolid && clickInit) orbBuffer = true;
        else if (botOnSolid) orbBuffer = false;
        click();
    }

    clickInit = false;
    hoveredOrb = std::nullopt;
}

// RENDER
void Player::render() {
    static GameData* gameData = game->getData();

    if (game->getState().main != LEVEL_DEAD) {
        renderMainTexture();
        if (gameData->other->iconSecondTexture[gamemode]) renderSecondTexture();
    }

    if (game->getMegahack()->getHack("show-hitboxes")->active) renderHitboxes();
    renderPracticeCheckpoints();
}

void Player::renderMainTexture() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    LevelSize iconSize = gameData->sizes->mainIcon[gamemode][size];
    LevelOffset iconOffset = gameData->offsets->mainIcon[gamemode][size];
    LevelPos iconPos = { pos.x + iconOffset.x, pos.y + iconOffset.y };
    std::string gamemodeStrigified = gameData->other->gamemodeStringified[gamemode];
    H2DE_Pos center = calculator->convertToPx(LevelOffset{ -iconOffset.x + gameData->sizes->redHitbox[gamemode][size].w / 2, iconOffset.y + gameData->sizes->redHitbox[gamemode][size].h / 2 });

    H2DE_GraphicObject* col1 = H2DE_CreateGraphicObject();
    col1->type = IMAGE;
    col1->texture = gamemodeStrigified + "-" + std::to_string(icons.ids[gamemode]) + "-1.png";
    col1->pos = calculator->convertToPx(iconPos, iconSize, false, false);
    col1->size = calculator->convertToPx(iconSize);
    col1->rotation = rotation;
    col1->rotationOrigin = center;
    col1->rgb = (H2DE_RGB)(gameData->colors->icons[icons.colorIDs[0]]);
    col1->index = Zindex{ T1, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, col1);

    H2DE_GraphicObject* col2 = H2DE_CreateGraphicObject(*col1);
    col2->texture = gamemodeStrigified + "-" + std::to_string(icons.ids[gamemode]) + "-2.png";
    col2->rgb = (H2DE_RGB)(gameData->colors->icons[icons.colorIDs[1]]);
    col2->index = Zindex{ T1, 1 }.getIndex();
    H2DE_AddGraphicObject(engine, col2);
}

void Player::renderSecondTexture() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    LevelSize iconSize = { gameData->sizes->secondIcon[gamemode][size].w, gameData->sizes->secondIcon[gamemode][size].h };
    LevelOffset iconOffset = gameData->offsets->secondIcon[gamemode][size];
    LevelPos iconPos = { pos.x + iconOffset.x, pos.y + iconOffset.y };
    H2DE_Pos center = calculator->convertToPx(LevelOffset{ -iconOffset.x + gameData->sizes->redHitbox[gamemode][size].w / 2, iconSize.h + iconOffset.y - gameData->sizes->redHitbox[gamemode][size].h / 2 });

    H2DE_GraphicObject* col1 = H2DE_CreateGraphicObject();
    col1->type = IMAGE;
    col1->texture = "cube-0-1.png";
    col1->pos = calculator->convertToPx(iconPos, iconSize, false, false);
    col1->size = calculator->convertToPx(iconSize);
    col1->rotation = rotation;
    col1->rotationOrigin = center;
    col1->rgb = (H2DE_RGB)(gameData->colors->icons[icons.colorIDs[0]]);
    col1->index = Zindex{ T1, -2 }.getIndex();
    H2DE_AddGraphicObject(engine, col1);

    H2DE_GraphicObject* col2 = H2DE_CreateGraphicObject(*col1);
    col2->texture = "cube-0-2.png";
    col2->rgb = (H2DE_RGB)(gameData->colors->icons[icons.colorIDs[1]]);
    col2->index = Zindex{ T1, -1 }.getIndex();
    H2DE_AddGraphicObject(engine, col2);
}

void Player::renderHitboxes() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    H2DE_Size absRedHitboxSize = calculator->convertToPx(gameData->sizes->redHitbox[gamemode][size]);
    LevelPos offsetRedHitboxPos = { pos.x + gameData->offsets->redHitbox[gamemode][size].x, pos.y + gameData->offsets->redHitbox[gamemode][size].y };
    
    H2DE_GraphicObject* redHitbox = H2DE_CreateGraphicObject();
    redHitbox->type = POLYGON;
    redHitbox->pos = calculator->convertToPx(offsetRedHitboxPos, gameData->sizes->redHitbox[gamemode][size], false, false);
    redHitbox->points = {
        { 0, 0 },
        { absRedHitboxSize.w, 0 },
        { absRedHitboxSize.w, absRedHitboxSize.h },
        { 0, absRedHitboxSize.h },
    };
    redHitbox->rgb = static_cast<H2DE_RGB>(gameData->colors->hitboxes[OBSTACLE]);
    redHitbox->index = Zindex{ H, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, redHitbox);

    LevelPos offsetBlueHitboxPos = { pos.x + gameData->offsets->blueHitbox[gamemode][size].x, pos.y + gameData->offsets->blueHitbox[gamemode][size].y };
    H2DE_Size blueHitboxSize = calculator->convertToPx(gameData->sizes->blueHitbox[gamemode][size]);

    H2DE_GraphicObject* blueHitbox = H2DE_CreateGraphicObject(*redHitbox);
    blueHitbox->pos = calculator->convertToPx(offsetBlueHitboxPos, gameData->sizes->blueHitbox[gamemode][size], false, false);
    blueHitbox->points = {
        { 0, 0 },
        { blueHitboxSize.w, 0 },
        { blueHitboxSize.w, blueHitboxSize.h },
        { 0, blueHitboxSize.h },
    };
    blueHitbox->rgb = static_cast<H2DE_RGB>(gameData->colors->hitboxes[SOLID]);
    blueHitbox->index = Zindex{ H, 1 }.getIndex();
    H2DE_AddGraphicObject(engine, blueHitbox);
}

void Player::renderPracticeCheckpoints() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    for (Checkpoint* c : practiceCheckpoints) {
        H2DE_GraphicObject* checkpoint = H2DE_CreateGraphicObject();
        checkpoint->type = IMAGE;
        checkpoint->texture = "practice-checkpoint.png";
        checkpoint->pos = calculator->convertToPx({ c->playerPos.x + gameData->offsets->checkpoint.x, c->playerPos.y + gameData->offsets->checkpoint.y }, gameData->sizes->checkpoint, false, false);
        checkpoint->size = calculator->convertToPx(gameData->sizes->checkpoint);
        checkpoint->index = Zindex{ T1, -2 }.getIndex();
        H2DE_AddGraphicObject(engine, checkpoint);
    }
}

// EVENTS
void Player::click() {
    static GameData* gameData = game->getData();

    if (hoveredOrb.has_value() && orbBuffer) {
        Orb orb;
        switch (hoveredOrb.value()->getData()->specialData.desc) {
            case SD_YELLOW: orb = YELLOW_ORB; break;
            case SD_PINK: orb = PINK_ORB; break;
            case SD_BLUE: orb = BLUE_ORB; break;
            default: orb = YELLOW_ORB; break;
        }

        orbBuffer = false;

        if (orb == BLUE_ORB) gravity = ((gravity == UPSIDE_DOWN) ? RIGHT_SIDE_UP : UPSIDE_DOWN);
        velocity.y = gameData->physics->orbs[orb][gamemode][size];
        hoveredOrb.value()->setUsed();

        jumps++;

    } else switch (gamemode) {
        case CUBE: if (botOnSolid) {
            if (gameData->physics->clicks[CUBE][size] * gravity > velocity.y) {
                velocity.y = gameData->physics->clicks[CUBE][size] * gravity;
                jumps++;
            }
        } break;
        case SHIP: velocity.y += (gameData->physics->clicks[SHIP][size] * gravity); break;
    }
}

void Player::kill() {
    static H2DE_Engine* engine = game->getEngine();

    game->setState({ LEVEL_DEAD, DEFAULT });
    if (level->getMode() == NORMAL_MODE) H2DE_PauseSound(engine, 0);
    H2DE_PlaySound(engine, 1, "death-sound.ogg", 0);

    Game::delay(1000, [this]() { level->respawn(); });
}

void Player::reset(Checkpoint* c) {
    static GameData* gameData = game->getData();

    if (level->getMode() == NORMAL_MODE) level->setBestNormalMode(percentage);
    else level->setBestPracticeMode(percentage);

    pos.x = c->playerPos.x;
    pos.y = c->playerPos.y;

    velocity.x = c->velocity.x;
    velocity.y = c->velocity.y;

    gravity = c->gravity;
    size = c->size;

    float yGamemode = (c->botGroundPosY == -1.0f) ? ceil((c->botGroundPosY + gameData->sizes->gamemodeHeights[c->gamemode]) / 2) : pos.y;
    setGamemode(c->gamemode, yGamemode, 0);

    rotation = 0;
    percentage = 0;

    botOnSolid = false;
    topOnSolid = false;
    clickInit = false;
    orbBuffer = false;

    startingItem = 0;
    collidedBlockFace.clear();
}

// PRACTICE MODE
void Player::addCheckpoint() {
    static Camera* camera = game->getCamera();

    Checkpoint* c = new Checkpoint();
    c->playerPos = pos;
    c->velocity = velocity;
    c->size = size;
    c->gamemode = gamemode;
    c->gravity = gravity;
    c->rotation = rotation;
    c->speed = level->getCurrentSpeed();
    c->camPos = camera->getPos();

    practiceCheckpoints.push_back(c);
}

void Player::removeLastCheckpoint() {
    if (!practiceCheckpoints.empty()) {
        delete practiceCheckpoints[practiceCheckpoints.size() - 1];
        practiceCheckpoints.pop_back();
    }
}

void Player::clearCheckpoints() {
    for (Checkpoint* c : practiceCheckpoints) delete c;
    practiceCheckpoints.clear();
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

Size Player::getSize() const {
    return size;
}

Gravity Player::getGravity() const {
    return gravity;
}

Checkpoint* Player::getLastPracticeCheckpoint() const {
    if (!practiceCheckpoints.empty()) return practiceCheckpoints[practiceCheckpoints.size() - 1];
    else return nullptr;
}

// SETTER
void Player::setClicking(bool value) {
    clicking = value;
    clickInit = value;
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
    rotation = 0;
    int gamemodeHeight = gameData->sizes->gamemodeHeights[g];

    float newTopGroundPosY, newBotGroundPosY;
    if (gamemodeHeight != -1) {
        if (y < 5.0f) y = 5.0f;

        float winHeight = winSize.h / round(winSize.w / BLOCKS_ON_WIDTH);
        newTopGroundPosY = floor((gamemodeHeight - 1.0f) / 2.0f) + y + 1.0f;
        newBotGroundPosY = newTopGroundPosY - gamemodeHeight;

        camera->setPos({ camPos.x, newBotGroundPosY - (winHeight - gamemodeHeight) / 2 }, ms);
    } else {
        newTopGroundPosY = gameData->positions->topGround.y;
        newBotGroundPosY = gameData->positions->botGround.y + gameData->sizes->ground.h;
    }

    level->setTopGroundPos({ topGroundPos.x, newTopGroundPosY }, ms);
    level->setBotGroundPos({ botGroundPos.x, newBotGroundPosY - gameData->sizes->ground.h }, ms);
}

void Player::setGravity(Gravity g) {
    gravity = g;
}

void Player::setYvelocity(float vy) {
    velocity.y = vy;
}

void Player::setHoveredOrb(Block* block) {
    hoveredOrb = block;
}
