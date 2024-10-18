#include "item.h"

// ITEM

// INIT
Item::Item(Game* g) : game(g), used(false) {

}

// CLEANUP
Item::~Item() {

}

// SETTER
void Item::setUsed() {
    used = true;
}


// BLOCK

// INIT
Block::Block(Game* game, BufferedBlock* d) : Item(game), data(d) {
    static H2DE_Engine* engine = game->getEngine();

    if (data->sprites != 0) {
        H2DE_Timeline* t = H2DE_CreateTimeline(engine, 500, LINEAR, [this](float blend) {
            this->currentSprite = std::min((int)floor(lerp(0, this->data->sprites, blend)), this->data->sprites - 1) + 1;
        }, NULL, -1);

        H2DE_AddTimelineToManager(tm, t);
    }
}

// CLEANUP
Block::~Block() {
    H2DE_DestroyTimelineManager(tm);

}

// UPDATE
void Block::update() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    Level* level = game->getLevel();
    float camPosX = game->getCamera()->getPos().x;

    static float camWidth = gameData->sizes->camera.w;
    static float camXremain = gameData->sizes->cameraXremain;
    static float blockEffectPadding = gameData->sizes->blockEffectPadding;
    static float moveOffset = gameData->offsets->blockEffectMove;

    float maxCamX = camPosX + camWidth;
    bool onScreen = (maxCamX > data->pos.x);

    BlockEffect levelBlockEffect = level->getBlockEffect();

    if (onScreen) {
        bool onMaxXedge = (data->pos.x >= maxCamX - blockEffectPadding);
        bool onMinXedge = (data->pos.x - blockEffectPadding <= camPosX);

        float blend, minOffset = 0.0f, maxOffset = 0.0f, minSize = 0.0f, maxSize = 0.0f;
        if (onMaxXedge) {
            if (!effect.has_value()) effect = levelBlockEffect;

            float dist = maxCamX - data->pos.x - camXremain;
            blend = std::clamp(dist / (blockEffectPadding - 0.5f), 0.0f, 1.0f);
            textureOpacity = lerp(SDL_MIN_UINT8, SDL_MAX_UINT8, blend);
            minOffset = moveOffset;
            maxOffset = 0.0f;
            minSize = 0.0f;
            maxSize = 1.0f;

        } else if (onMinXedge) {
            if (!effect.has_value()) effect = levelBlockEffect;

            float dist = camPosX + blockEffectPadding - 1.0f - data->pos.x;
            blend = std::clamp(dist / (blockEffectPadding - 0.5f), 0.0f, 1.0f);
            textureOpacity = lerp(SDL_MAX_UINT8, SDL_MIN_UINT8, blend);
            minOffset = 0.0f;
            maxOffset = moveOffset;
            minSize = 1.0f;
            maxSize = 0.0f;

        } else effect = std::nullopt;
        
        if (effect.has_value()) switch (effect.value()) {
             case FROM_TOP: textureOffset = { 0.0f, lerp(minOffset, maxOffset, blend) }; break;
             case FROM_BOTTOM: textureOffset = { 0.0f, lerp(-minOffset, -maxOffset, blend) }; break;
             case FROM_LEFT: textureOffset = { lerp(-minOffset, -maxOffset, blend), 0.0f }; break;
             case FROM_RIGHT: textureOffset = { lerp(minOffset, maxOffset, blend), 0.0f }; break;
             case SCALE: textureScale = lerp(minSize, maxSize, blend); break;
        }
    } else textureOpacity = 0;
}

// RENDER
void Block::render() {
    LevelPos camPos = game->getCamera()->getPos();
    GameState gameState = game->getState();

    if (!pickedUp) if (data->pos.x + data->textureSize.w >= camPos.x && data->pos.x - data->textureSize.w <= camPos.x + BLOCKS_ON_WIDTH + 1) { // refaire
        if (gameState.main != LEVEL_PAUSE) H2DE_TickTimelineManager(tm);
        renderTexture();
        if (data->glowID != 0) renderGlow();
        if (game->getMegahack()->getHack("show-hitboxes")->active) renderHitbox();
    }
}

void Block::renderTexture() {
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();
    Level* level = game->getLevel();
    LevelPos camPos = game->getCamera()->getPos();

    std::string tex = data->texture.substr(0, data->texture.size() - 4).append(((data->sprites != 0) ? std::string("-").append(std::to_string(currentSprite)) : "")).append(".png");
    H2DE_Size absTexOrigin = calculator->convertToPx(LevelSize{ data->texOrigin.x, data->texOrigin.y });
    H2DE_RGB rgb = static_cast<H2DE_RGB>(level->getData()->colors[data->colorID]);
    rgb.a = textureOpacity;

    H2DE_GraphicObject* texture = H2DE_CreateGraphicObject();
    texture->type = IMAGE;
    texture->texture = tex;
    texture->pos = calculator->convertToPx({ data->pos.x + data->textureOffset.x + textureOffset.x, data->pos.y + data->textureOffset.y + textureOffset.y }, data->textureSize, false, false);
    texture->size = calculator->convertToPx(data->textureSize);
    texture->rotation = data->rotation;
    texture->rotationOrigin = { absTexOrigin.w, absTexOrigin.h };
    texture->scale = { textureScale, textureScale };
    texture->scaleOrigin = { absTexOrigin.w, absTexOrigin.h };
    texture->flip = data->flip;
    texture->rgb = rgb;
    texture->index = data->zIndex->getIndex();
    H2DE_AddGraphicObject(engine, texture);
}

void Block::renderGlow() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    static float glowOpacity = gameData->colors->glowOpacity;
    H2DE_Size absGlowOrigin = calculator->convertToPx(LevelSize{ data->glowOrigin.x, data->glowOrigin.y });
    H2DE_RGB rgb = { 255, 255, 255, 255 }; // replace => pcol 1 or 2 (idk)
    rgb.a = textureOpacity * glowOpacity;

    H2DE_GraphicObject* glow = H2DE_CreateGraphicObject();
    glow->type = IMAGE;
    glow->texture = "glow-" + std::to_string(data->glowID) + ".png";
    glow->pos = calculator->convertToPx({ data->pos.x + textureOffset.x + data->glowOffset.x, data->pos.y + textureOffset.y + data->glowOffset.y }, data->glowSize, false, false);
    glow->size = calculator->convertToPx(data->glowSize);
    glow->rotation = data->rotation;
    glow->rotationOrigin = { absGlowOrigin.w, absGlowOrigin.h };
    glow->scale = { textureScale, textureScale };
    glow->scaleOrigin = { absGlowOrigin.w, absGlowOrigin.h };
    glow->flip = data->flip;
    glow->rgb = rgb;
    glow->index = data->zIndex->getIndex() - 1;
    H2DE_AddGraphicObject(engine, glow);
}

void Block::renderHitbox() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    LevelPos camPos = game->getCamera()->getPos();

    if (data->type != DECORATION) {
        H2DE_Size absHitboxSize = calculator->convertToPx(data->hitboxSize);
        H2DE_Pos absHitboxOffset = calculator->convertToPx(data->hitboxOffset);
        H2DE_GraphicObject* hitbox = H2DE_CreateGraphicObject();
        hitbox->type = POLYGON;
        hitbox->pos = calculator->convertToPx({ data->pos.x + data->hitboxOffset.x, data->pos.y + data->hitboxOffset.y }, data->hitboxSize, false, false);
        hitbox->points = {
            { 0, 0, },
            { absHitboxSize.w, 0, },
            { absHitboxSize.w, absHitboxSize.h },
            { 0, absHitboxSize.h },
        };
        hitbox->rgb = static_cast<H2DE_RGB>(gameData->colors->hitboxes[data->type]);
        hitbox->index = Zindex{ H, -1 }.getIndex();
        H2DE_AddGraphicObject(engine, hitbox);
    }
}

// RESET
void Block::reset() {
    used = false;
    pickedUp = false;

    textureOpacity = SDL_MAX_UINT8;
    textureOffset = { 0.0f, 0.0f };
    textureScale = 1;
    effect = std::nullopt;

    H2DE_ClearTimelineManager(tm);
}

// GETTER
BufferedBlock* Block::getData() const {
    return data;
}
 
bool Block::entered() const {
    return used;
}

bool Block::isPickedUp() const {
    return pickedUp;
}

// SETTER
void Block::enter() {
    GameData* gameData = game->getData();
    Player* player = game->getLevel()->getPlayer();

    Gamemode gm = player->getGamemode();
    Size si = player->getSize();
    Gravity gr = player->getGravity();

    switch (data->specialData.type) {
        case SD_PORTAL: switch (data->specialData.desc) {
            case SD_CUBE: player->setGamemode(CUBE, data->pos.y, 500); used = true; break;
            case SD_SHIP: player->setGamemode(SHIP, data->pos.y, 500); used = true; break;
            case SD_RIGHT_SIDE_UP: player->setGravity(RIGHT_SIDE_UP); used = true; break;
            case SD_UPSIDE_DOWN: player->setGravity(UPSIDE_DOWN); used = true; break;
        } break;
        case SD_ORB: player->setHoveredOrb(this); break;
        case SD_PAD: switch (data->specialData.desc) {
            case SD_YELLOW: player->setYvelocity(gameData->physics->pads[YELLOW_PAD][gm][si] * gr); used = true; break;
            case SD_PINK: player->setYvelocity(gameData->physics->pads[PINK_PAD][gm][si] * gr); used = true; break;
            case SD_BLUE: player->setYvelocity(gameData->physics->pads[BLUE_PAD][gm][si] * gr); player->setGravity(((gr == UPSIDE_DOWN) ? RIGHT_SIDE_UP : UPSIDE_DOWN)); used = true; break;
        } break;
        case SD_COIN: switch (data->specialData.desc) {
            case SD_SECRET: pickedUp = true; used = true; break;
        } break;
    }
}



// TRIGGER

// INIT
Trigger::Trigger(Game* game, BufferedTrigger* d) : Item(game), data(d) {
    if (data->type == STARTPOS) used = true;
}

// CLEANUP
Trigger::~Trigger() {
    H2DE_DestroyTimelineManager(tm);
}

// UPDATE
void Trigger::update() {
    static GameData* gameData = game->getData();
    Player* player = game->getLevel()->getPlayer();

    LevelPos playerPos = player->getPos();
    LevelSize playerSize = gameData->sizes->redHitbox[player->getGamemode()][player->getSize()];

    H2DE_TickTimelineManager(tm);

    if (used) return;
    if (playerPos.x + playerSize.w >= data->pos.x) {
        if (data->touchTrigger) {

            Rect playerRect = { playerPos.x, playerPos.y, playerSize.w, playerSize.h };
            Rect triggerRect = { data->pos.x, data->pos.y, 1.0f, 1.0f };

            if (Rect::intersect(&playerRect, &triggerRect)) trigger();
        } else trigger();
    }
}

void Trigger::trigger() {
    static H2DE_Engine* engine = game->getEngine();
    static std::vector<TriggerType> colorTriggers = game->getData()->other->colorTriggers;
    static std::vector<TriggerType> blockEffectTriggers = game->getData()->other->blockEffectTriggers;
    Level* level = game->getLevel();

    for (Item* item : *(level->getItems())) {
        Trigger* trigger = ItemManager::castToTrigger(item);
        if (!trigger) continue;
        if (trigger->data->pos.x > data->pos.x) break;
        if (trigger->data->type == data->type) H2DE_ClearTimelineManager(trigger->tm);
    }

    used = true;

    if (std::find(colorTriggers.begin(), colorTriggers.end(), data->type) != colorTriggers.end()) {
        Color sCol;
        switch (data->type) {
            case BACKGROUND: sCol = level->getBackgroundColor(); break;
            case GROUND: sCol = level->getGroundColor(); break;
            case LINE: sCol = level->getLineColor(); break;
            default: sCol = { 0, 0, 0, 0 }; break;
        }

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, data->ms, LINEAR, [this, level, sCol](float blend) {
            Color eCol = data->color;
            Color bCol = {
                static_cast<Uint8>(lerp(sCol.r, eCol.r, blend)),
                static_cast<Uint8>(lerp(sCol.g, eCol.g, blend)),
                static_cast<Uint8>(lerp(sCol.b, eCol.b, blend)),
                static_cast<Uint8>(lerp(sCol.a, eCol.a, blend)),
            };

            switch (data->type) {
                case BACKGROUND: level->setBackgroundColor(bCol); break;
                case GROUND: level->setGroundColor(bCol); break;
                case LINE: level->setLineColor(bCol); break;
                default: break;
            }
        }, NULL, 0);
        H2DE_AddTimelineToManager(tm, t);

    } else if (std::find(blockEffectTriggers.begin(), blockEffectTriggers.end(), data->type) != blockEffectTriggers.end()) {
        switch (data->type) {
            case BLOCK_FADE: level->setBlockEffect(FADE); break;
            case BLOCK_FROM_TOP: level->setBlockEffect(FROM_TOP); break;
            case BLOCK_FROM_BOTTOM: level->setBlockEffect(FROM_BOTTOM); break;
            case BLOCK_FROM_LEFT: level->setBlockEffect(FROM_LEFT); break;
            case BLOCK_FROM_RIGHT: level->setBlockEffect(FROM_RIGHT); break;
            case BLOCK_SCALE: level->setBlockEffect(SCALE); break;
        }
    }
}

// RENDER
void Trigger::render() {

}

// RESET
void Trigger::reset() {
    if (data->type != STARTPOS) used = false;
    H2DE_ClearTimelineManager(tm);
}

// GETTER
BufferedTrigger* Trigger::getData() const {
    return data;
}



// ITEM_MANAGER
Block* ItemManager::castToBlock(Item* item) {
    return dynamic_cast<Block*>(item);
}
 
Trigger* ItemManager::castToTrigger(Item* item) {
    return dynamic_cast<Trigger*>(item);
}
