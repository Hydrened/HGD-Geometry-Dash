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
Block::Block(Game* game, BufferedBlock* b) : Item(game), bb(b) {
    static H2DE_Engine* engine = game->getEngine();

    if (bb->data->baseTexture.has_value()) texturesData.push_back(bb->data->baseTexture.value());
    if (bb->data->detailTexture.has_value()) texturesData.push_back(bb->data->detailTexture.value());

    for (BlockTextureData* textureData : texturesData) {
        if (textureData->sprites != 0) {
            H2DE_Timeline* t = H2DE_CreateTimeline(engine, 500, LINEAR, [this, textureData](float blend) {
                this->currentSprite = std::min((int)floor(lerp(0, textureData->sprites, blend)), textureData->sprites - 1) + 1;
            }, NULL, -1);
            H2DE_AddTimelineToManager(tm, t);
        }
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
    bool onScreen = (maxCamX > bb->pos.x);

    BlockEffect levelBlockEffect = level->getBlockEffect();

    if (onScreen) {
        bool onMaxXedge = (bb->pos.x >= maxCamX - blockEffectPadding);
        bool onMinXedge = (bb->pos.x - blockEffectPadding <= camPosX);

        float blend, minOffset = 0.0f, maxOffset = 0.0f, minSize = 0.0f, maxSize = 0.0f;
        if (onMaxXedge) {
            if (!effect.has_value()) effect = levelBlockEffect;

            float dist = maxCamX - bb->pos.x - camXremain;
            blend = std::clamp(dist / (blockEffectPadding - 0.5f), 0.0f, 1.0f);
            textureOpacity = lerp(SDL_MIN_UINT8, SDL_MAX_UINT8, blend);
            minOffset = moveOffset;
            maxOffset = 0.0f;
            minSize = 0.0f;
            maxSize = 1.0f;

        } else if (onMinXedge) {
            if (!effect.has_value()) effect = levelBlockEffect;

            float dist = camPosX + blockEffectPadding - 1.0f - bb->pos.x;
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
             default: break;
        }
    } else textureOpacity = 0;
}

// RENDER
void Block::render() {
    LevelPos camPos = game->getCamera()->getPos();
    GameState gameState = game->getState();

    // replace
    if (!pickedUp) {
        for (BlockTextureData* textureData : texturesData) {
            if (bb->pos.x + textureData->size.w >= camPos.x && bb->pos.x - textureData->size.w <= camPos.x + BLOCKS_ON_WIDTH + 1) {
                if (gameState.main != LEVEL_PAUSE) H2DE_TickTimelineManager(tm);
                renderTexture(textureData);
                if (bb->data->glow.has_value()) renderGlow();
                if (game->getMegahack()->getHack("show-hitboxes")->active && bb->data->type != DECORATION) renderHitbox();
            }
        }
    }
}

void Block::renderTexture(BlockTextureData* td) {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    Level* level = game->getLevel();
    LevelPos camPos = game->getCamera()->getPos();

    BlockTextureData* bt = (bb->data->baseTexture.has_value()) ? bb->data->baseTexture.value() : nullptr;
    BlockTextureData* dt = (bb->data->detailTexture.has_value()) ? bb->data->detailTexture.value() : nullptr;
    LevelColor color;
    if (bt == td) {
        if (bb->baseColor.has_value()) color = bb->baseColor.value();
        else color = td->color;
    } else if (dt == td) {
        if (bb->detailColor.has_value()) color = bb->detailColor.value();
        else color = td->color;
    } else color = td->color;

    H2DE_RGB rgb;
    switch (color) {
        case COL_WHITE: rgb = { 255, 255, 255, 255 }; break;
        case COL_BLACK: rgb = { 0, 0, 0, 255 }; break;
        case COL_OBJ: rgb = { 255, 255, 255, 255 }; break;
        case COL_1: rgb = (H2DE_RGB)level->getData()->colors[0]; break;
        case COL_2: rgb = (H2DE_RGB)level->getData()->colors[1]; break;
        case COL_3: rgb = (H2DE_RGB)level->getData()->colors[2]; break;
        case COL_4: rgb = (H2DE_RGB)level->getData()->colors[3]; break;
        case COL_P_1: rgb = (H2DE_RGB)gameData->colors->icons[level->getPlayer()->getIcons()->colorIDs[0]]; break;
        case COL_P_2: rgb = (H2DE_RGB)gameData->colors->icons[level->getPlayer()->getIcons()->colorIDs[1]]; break;
        default: break;
    }
    rgb.a = textureOpacity;
    
    std::string tex = td->texture.substr(0, td->texture.size() - 4).append(((td->sprites != 0) ? std::string("-").append(std::to_string(currentSprite)) : "")).append(".png");
    H2DE_Size absOrigin = calculator->convertToPx(LevelSize{ td->origin.x, td->origin.y });

    H2DE_GraphicObject* texture = H2DE_CreateGraphicObject();
    texture->type = IMAGE;
    texture->texture = tex;
    texture->pos = calculator->convertToPx({ bb->pos.x + td->offset.x + textureOffset.x, bb->pos.y + td->offset.y + textureOffset.y }, td->size, false, false);
    texture->size = calculator->convertToPx(td->size);
    texture->rotation = bb->rotation;
    texture->rotationOrigin = { absOrigin.w, absOrigin.h };
    texture->scale = { textureScale, textureScale };
    texture->scaleOrigin = { absOrigin.w, absOrigin.h };
    texture->flip = bb->flip;
    texture->rgb = rgb;
    texture->index = bb->data->zIndex->getIndex();
    H2DE_AddGraphicObject(engine, texture);
}

void Block::renderGlow() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    static float glowOpacity = gameData->colors->glowOpacity;

    BlockGlowData* g = bb->data->glow.value();

    H2DE_Size absOrigin = calculator->convertToPx(LevelSize{ g->origin.x, g->origin.y });
    H2DE_RGB rgb = { 255, 255, 255, 255 };
    rgb.a = textureOpacity * glowOpacity;

    H2DE_GraphicObject* glow = H2DE_CreateGraphicObject();
    glow->type = IMAGE;
    glow->texture = "glow-" + std::to_string(g->id) + ".png";
    glow->pos = calculator->convertToPx({ bb->pos.x + textureOffset.x + g->offset.x, bb->pos.y + textureOffset.y + g->offset.y }, g->size, false, false);
    glow->size = calculator->convertToPx(g->size);
    glow->rotation = bb->rotation;
    glow->rotationOrigin = { absOrigin.w, absOrigin.h };
    glow->scale = { textureScale, textureScale };
    glow->scaleOrigin = { absOrigin.w, absOrigin.h };
    glow->flip = bb->flip;
    glow->rgb = rgb;
    glow->index = Zindex{ B4, -99 }.getIndex();
    H2DE_AddGraphicObject(engine, glow);
}

void Block::renderHitbox() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    LevelPos camPos = game->getCamera()->getPos();

    BlockHitboxData* h = bb->data->hitboxData.value();
    H2DE_Size absSize = calculator->convertToPx(h->size);

    H2DE_GraphicObject* hitbox = H2DE_CreateGraphicObject();
    hitbox->type = POLYGON;
    hitbox->pos = calculator->convertToPx({ bb->pos.x + h->offset.x, bb->pos.y + h->offset.y }, h->size, false, false);
    hitbox->points = {
        { 0, 0, },
        { absSize.w, 0, },
        { absSize.w, absSize.h },
        { 0, absSize.h },
    };
    hitbox->rotation = bb->rotation;
    hitbox->rotationOrigin = calculator->convertToPx(LevelOffset{ h->origin.x, h->origin.y });
    hitbox->rgb = (H2DE_RGB)(gameData->colors->hitboxes[bb->data->type]);
    hitbox->index = Zindex{ H, -1 }.getIndex();
    H2DE_AddGraphicObject(engine, hitbox);
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
BufferedBlock* Block::getBufferedData() const {
    return bb;
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

    BlockSpecialData* sd = bb->data->specialData.value();

    switch (sd->type) {
        case SD_PORTAL: switch (sd->desc) {
            case SD_CUBE: player->setGamemode(CUBE, bb->pos.y, 500); used = true; break;
            case SD_SHIP: player->setGamemode(SHIP, bb->pos.y, 500); used = true; break;
            case SD_RIGHT_SIDE_UP: player->setGravity(RIGHT_SIDE_UP); used = true; break;
            case SD_UPSIDE_DOWN: player->setGravity(UPSIDE_DOWN); used = true; break;
            default: break;
        } break;

        case SD_ORB: player->setHoveredOrb(this); break;

        case SD_PAD: switch (sd->desc) {
            case SD_YELLOW: player->setYvelocity(gameData->physics->pads[YELLOW_PAD][gm][si] * gr); used = true; break;
            case SD_PINK: player->setYvelocity(gameData->physics->pads[PINK_PAD][gm][si] * gr); used = true; break;
            case SD_BLUE: player->setYvelocity(gameData->physics->pads[BLUE_PAD][gm][si] * gr); player->setGravity(((gr == UPSIDE_DOWN) ? RIGHT_SIDE_UP : UPSIDE_DOWN)); used = true; break;
            default: break;
        } break;

        case SD_COIN: switch (sd->desc) {
            case SD_SECRET: pickedUp = true; used = true; break;
            default: break;
        } break;
        default: break;
    }
}



// TRIGGER

// INIT
Trigger::Trigger(Game* game, BufferedTrigger* b) : Item(game), bt(b) {
    if (bt->data->type == T_STARTPOS) used = true;
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
    if (playerPos.x + playerSize.w >= bt->pos.x) {
        if (bt->touchTrigger) {

            Rect playerRect = { playerPos.x, playerPos.y, playerSize.w, playerSize.h };
            Rect triggerRect = { bt->pos.x, bt->pos.y, 1.0f, 1.0f };

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
        if (trigger->bt->pos.x > bt->pos.x) break;
        if (trigger->bt->data->type == bt->data->type) H2DE_ClearTimelineManager(trigger->tm);
    }

    used = true;

    if (std::find(colorTriggers.begin(), colorTriggers.end(), bt->data->type) != colorTriggers.end()) {
        Color sCol;
        switch (bt->data->type) {
            case T_BACKGROUND: sCol = level->getBackgroundColor(); break;
            case T_GROUND: sCol = level->getGroundColor(); break;
            case T_LINE: sCol = level->getLineColor(); break;
            default: sCol = { 0, 0, 0, 0 }; break;
        }

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, bt->ms.value(), LINEAR, [this, level, sCol](float blend) {
            Color eCol = bt->color.value();
            Color bCol = {
                (Uint8)(lerp(sCol.r, eCol.r, blend)),
                (Uint8)(lerp(sCol.g, eCol.g, blend)),
                (Uint8)(lerp(sCol.b, eCol.b, blend)),
                (Uint8)(lerp(sCol.a, eCol.a, blend)),
            };

            switch (bt->data->type) {
                case T_BACKGROUND: level->setBackgroundColor(bCol); break;
                case T_GROUND: level->setGroundColor(bCol); break;
                case T_LINE: level->setLineColor(bCol); break;
                default: break;
            }
        }, NULL, 0);
        H2DE_AddTimelineToManager(tm, t);

    } else if (std::find(blockEffectTriggers.begin(), blockEffectTriggers.end(), bt->data->type) != blockEffectTriggers.end()) {
        switch (bt->data->type) {
            case T_BE_FADE: level->setBlockEffect(FADE); break;
            case T_BE_FROM_TOP: level->setBlockEffect(FROM_TOP); break;
            case T_BE_FROM_BOTTOM: level->setBlockEffect(FROM_BOTTOM); break;
            case T_BE_FROM_LEFT: level->setBlockEffect(FROM_LEFT); break;
            case T_BE_FROM_RIGHT: level->setBlockEffect(FROM_RIGHT); break;
            case T_BE_SCALE: level->setBlockEffect(SCALE); break;
            default: break;
        }
    }
}

// RENDER
void Trigger::render() {

}

// RESET
void Trigger::reset() {
    if (bt->data->type != T_STARTPOS) used = false;
    H2DE_ClearTimelineManager(tm);
}

// GETTER
BufferedTrigger* Trigger::getBufferedData() const {
    return bt;
}



// ITEM_MANAGER
Block* ItemManager::castToBlock(Item* item) {
    return dynamic_cast<Block*>(item);
}
 
Trigger* ItemManager::castToTrigger(Item* item) {
    return dynamic_cast<Trigger*>(item);
}
