#include "items/block.h"

// INIT
Block::Block(Game* g, const ItemBuffer& ib, const BlockBuffer& bb) : Item(g, ib), blockBuffer(bb) {
    loadData();
    initObject();
}

void Block::loadData() {
    static const Data* gameData = game->getData();
    data = gameData->getBlock(itemBuffer.id);
}

void Block::initObject() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.pos = game->convertToLevelPos(itemBuffer.pos, { 1.0f, 1.0f });
    od.size = { 1.0f, 1.0f };
    od.pivot = od.size.getCenter();
    od.rotation = blockBuffer.rotation;
    od.flip = blockBuffer.flip;
    od.index = data.index;

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();

    for (int i = 0; i < data.surfaces.size(); i++) {
        const BlockData::SurfaceData& surface = data.surfaces[i];

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.textureName = surface.sheet;
        sd.rect = surface.dest;
        sd.color = surface.defaultColor;
        sd.scaleMode = H2DE_SCALE_MODE_BEST;

        H2DE_TextureData tod = H2DE_TextureData();
        tod.srcRect = surface.src;
        
        bod.surfaces[std::to_string(i)] = H2DE_CreateTexture(engine, sd, tod);
    }
    
    object = H2DE_CreateBasicObject(engine, od, bod);
}

// CLEANUP
Block::~Block() {
    destroyObject();
}

void Block::destroyObject() {
    game->destroyObjects({ object });
}

// UPDATE
void Block::update() {
    
}

// GETTER
bool Block::hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const {
    for (const BlockData::SurfaceData& surface : data.surfaces) {
        if (itemBuffer.pos.x + surface.dest.getPos().x + surface.dest.getSize().x < camPos.x) {
            return true;
        }
    }

    return false;
}
