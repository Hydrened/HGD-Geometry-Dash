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
            this->currentSprite = std::min((int)floor(blend * this->data->sprites), this->data->sprites - 1) + 1;
        }, NULL, -1);

        H2DE_AddTimelineToManager(sprites, t);
    }
}

// CLEANUP
Block::~Block() {
    H2DE_DestroyTimelineManager(sprites);

}

// UPDATE
void Block::update() {

}

// RENDER
void Block::render() {
    LevelPos camPos = game->getCamera()->getPos();
    GameState gameState = game->getState();

    if (!pickedUp) if (data->pos.x + data->textureSize.w >= camPos.x && data->pos.x - data->textureSize.w <= camPos.x + BLOCKS_ON_WIDTH + 1) {
        if (gameState.main != LEVEL_PAUSE) H2DE_TickTimelineManager(sprites);
        
        renderTexture();
        if (game->getMegahack()->getHack("show-hitboxes")->active) renderHitbox();
    }
}

void Block::renderTexture() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    Level* level = game->getLevel();
    LevelPos playerPos = level->getPlayer()->getPos();
    LevelPos camPos = game->getCamera()->getPos();

    bool textureOnCamX = (data->pos.x + data->textureOffset.x + data->textureSize.w > camPos.x) && (data->pos.x + data->textureOffset.x < camPos.x + BLOCKS_ON_WIDTH + 1);
    if (textureOnCamX) {
        std::string tex = data->texture.substr(0, data->texture.size() - 4).append(((data->sprites != 0) ? std::string("-").append(std::to_string(currentSprite)) : "")).append(".png");
        H2DE_Size absTexOri = calculator->convertToPx(LevelSize{ data->rotationOrigin.x, data->rotationOrigin.y });

        H2DE_GraphicObject* texture = H2DE_CreateGraphicObject();
        texture->type = IMAGE;
        texture->texture = tex;
        texture->pos = calculator->convertToPx({ data->pos.x + data->textureOffset.x, data->pos.y + data->textureOffset.y }, data->textureSize, false, false);
        texture->size = calculator->convertToPx(data->textureSize);
        texture->rotation = data->rotation;
        texture->rotationOrigin = { absTexOri.w, absTexOri.h };
        texture->flip = data->flip;
        texture->color = static_cast<H2DE_Color>(level->getData()->colors[data->colorID]);
        texture->index = data->zIndex->getIndex();
        H2DE_AddGraphicObject(engine, texture);
    }
}

void Block::renderHitbox() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();
    LevelPos camPos = game->getCamera()->getPos();

    bool hitboxOnCamX = (data->pos.x + data->hitboxOffset.x + data->hitboxSize.w > camPos.x) && (data->pos.x + data->hitboxOffset.x < camPos.x + BLOCKS_ON_WIDTH + 1);
    if (hitboxOnCamX && data->type != DECORATION) {
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
        hitbox->color = static_cast<H2DE_Color>(gameData->colors->hitboxes[data->type]);
        hitbox->index = Zindex{ H, -1 }.getIndex();
        H2DE_AddGraphicObject(engine, hitbox);
    }
}

// RESET
void Block::reset() {
    used = false;
    pickedUp = false;
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
    H2DE_DestroyTimelineManager(effects);
}

// UPDATE
void Trigger::update() {
    static GameData* gameData = game->getData();
    Player* player = game->getLevel()->getPlayer();

    LevelPos playerPos = player->getPos();
    LevelSize playerSize = gameData->sizes->redHitbox[player->getGamemode()][player->getSize()];

    H2DE_TickTimelineManager(effects);

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
    Level* level = game->getLevel();

    for (Item* item : *(level->getItems())) {
        Trigger* trigger = ItemManager::castToTrigger(item);
        if (!trigger) continue;
        if (trigger->data->pos.x > data->pos.x) break;
        if (trigger->data->type == data->type) H2DE_ClearTimelineManager(trigger->effects);
    }

    used = true;

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
            static_cast<Uint8>(sCol.r + blend * (eCol.r - sCol.r)),
            static_cast<Uint8>(sCol.g + blend * (eCol.g - sCol.g)),
            static_cast<Uint8>(sCol.b + blend * (eCol.b - sCol.b)),
            static_cast<Uint8>(sCol.a + blend * (eCol.a - sCol.a)),
        };

        switch (data->type) {
            case BACKGROUND: level->setBackgroundColor(bCol); break;
            case GROUND: level->setGroundColor(bCol); break;
            case LINE: level->setLineColor(bCol); break;
            default: break;
        }
    }, NULL, 0);

    H2DE_AddTimelineToManager(effects, t);
}

// RENDER
void Trigger::render() {

}

// RESET
void Trigger::reset() {
    if (data->type != STARTPOS) used = false;
    H2DE_ClearTimelineManager(effects);
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
