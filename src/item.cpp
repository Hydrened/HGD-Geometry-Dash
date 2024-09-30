#include "item.h"

// ITEM

// INIT
Item::Item(Game* g) : game(g), used(false) {

}

// CLEANUP
Item::~Item() {

}



// BLOCK

// INIT
Block::Block(Game* game, BufferedBlock* d) : Item(game), data(d) {

}

// CLEANUP
Block::~Block() {

}

// UPDATE
void Block::update() {
    H2DE_TickTimelineManager(tm);
}

// RENDER
void Block::render() {
    LevelPos camPos = game->getCamera()->getPos();

    if (data->pos.x + data->textureSize.w >= camPos.x && data->pos.x - data->textureSize.w <= camPos.x + BLOCKS_ON_WIDTH + 1) {
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
        H2DE_Size absBlockSize = calculator->convertToPx(LevelSize{ 1.0f, 1.0f });

        H2DE_GraphicObject* texture = new H2DE_GraphicObject();
        texture->type = IMAGE;
        texture->texture = data->texture;
        texture->pos = calculator->convertToPx({ data->pos.x + data->textureOffset.x, data->pos.y + data->textureOffset.y }, data->textureSize, false, false);
        texture->size = calculator->convertToPx(data->textureSize);
        texture->rotation = data->rotation;
        texture->rotationOrigin = { absBlockSize.w / 2, absBlockSize.h / 2 };
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
        H2DE_GraphicObject* hitbox = new H2DE_GraphicObject();
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
    H2DE_ClearTimelineManager(tm);
    used = false;
}

// ACTIONS
void Block::enter() {
    Player* player = game->getLevel()->getPlayer();
    used = true;

    switch (data->specialData.type) {
        case SD_PORTAL: switch (data->specialData.desc) {
            case SD_CUBE: player->setGamemode(CUBE, data->pos.y); break;
            case SD_SHIP: player->setGamemode(SHIP, data->pos.y); break;
            case SD_RIGHT_SIDE_UP: break;
            case SD_UPSIDE_DOWN: break;
        } break;
        case SD_ORB: switch (data->specialData.desc) {
            case SD_PINK: break;
            case SD_YELLOW: break;
            case SD_BLUE: break;
        } break;
        case SD_PAD: switch (data->specialData.desc) {
            case SD_PINK: break;
            case SD_YELLOW: break;
            case SD_BLUE: break;
        } break;
        case SD_COIN: switch (data->specialData.desc) {
            case SD_SECRET: break;
        } break;
    }
}

// GETTER
BufferedBlock* Block::getData() {
    return data;
}
 
bool Block::entered() {
    return used;
}



// TRIGGER

// INIT
Trigger::Trigger(Game* game, BufferedTrigger* d) : Item(game), data(d) {
    if (data->type == STARTPOS) used = true;
}

// CLEANUP
Trigger::~Trigger() {
    
}

// UPDATE
void Trigger::update() {
    Player* player = game->getLevel()->getPlayer();

    H2DE_TickTimelineManager(tm);

    if (used) return;
    if (player->getPos().x >= data->pos.x) {
        if (data->touchTrigger) {

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
        if (trigger->data->type == data->type) H2DE_ClearTimelineManager(trigger->tm);
    }

    used = true;

    Color sCol;
    switch (data->type) {
        case BACKGROUND: sCol = level->getBackgroundColor(); break;
        case GROUND: sCol = level->getGroundColor(); break;
        case LINE: sCol = level->getLineColor(); break;
        default: sCol = { 0, 0, 0, 0 }; break;
    }

    H2DE_Timeline* timeline = H2DE_CreateTimeline(engine, data->ms, LINEAR, [this, level, sCol](float blend) {
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
    }, NULL);

    H2DE_AddTimelineToManager(tm, timeline);
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
BufferedTrigger* Trigger::getData() {
    return data;
}



// ITEM_MANAGER
Block* ItemManager::castToBlock(Item* item) {
    return dynamic_cast<Block*>(item);
}
 
Trigger* ItemManager::castToTrigger(Item* item) {
    return dynamic_cast<Trigger*>(item);
}
