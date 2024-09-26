#include "item.h"

// ITEM
// INIT
Item::Item(Game* g) : game(g), used(false) {

}

// CLEANUP
Item::~Item() {

}

// GETTER
bool Item::isUsed() {
    return used;
}

// SETTER
void Item::setUsed() {
    used = true;
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

}

// RENDER
void Block::render() {
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();
    LevelPos camPos = game->getCamera()->getPos();

    bool textureOnCamX = (data->pos.x + data->textureOffset.x + data->textureSize.w > camPos.x) && (data->pos.x + data->textureOffset.x < camPos.x + BLOCKS_ON_WIDTH + 1);
    bool hitboxOnCamX = (data->pos.x + data->hitboxOffset.x + data->hitboxSize.w > camPos.x) && (data->pos.x + data->hitboxOffset.x < camPos.x + BLOCKS_ON_WIDTH + 1);

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
        texture->color = static_cast<H2DE_Color>(game->getLevel()->getData()->colors[data->colorID]);
        texture->index = data->zIndex->getIndex();
        H2DE_AddGraphicObject(engine, texture);
    }
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

// GETTER
BufferedBlock* Block::getData() {
    return data;
}
 


// TRIGGER
// INIT
Trigger::Trigger(Game* game, BufferedTrigger* d) : Item(game), data(d) {
    
}

// CLEANUP
Trigger::~Trigger() {
    
}

// UPDATE
void Trigger::update() {

}

// RENDER
void Trigger::render() {

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
