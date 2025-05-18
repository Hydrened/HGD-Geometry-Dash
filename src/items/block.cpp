#include "items/block.h"

// INIT
Block::Block(Game* g, const ItemData& id, const BlockData& bd) : Item(g, id), blockData(bd) {
    initObject();
}

void Block::initObject() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = game->convertToLevelPos(itemData.pos, { 1.0f, 1.0f }).makeRect({ 1.0f, 1.0f });
    od.pivot = od.rect.getSize().getCenter();
    od.rotation = blockData.rotation;
    od.flip = blockData.flip;
    od.index = blockData.index;

    if (blockData.hitbox.has_value()) {
        const BlockData::HitboxData& hitboxData = blockData.hitbox.value();

        H2DE_Hitbox hitbox = H2DE_Hitbox();
        hitbox.rect = hitboxData.dest;
        hitbox.color = hitboxData.color;
        od.hitboxes["main"] = hitbox;
    }

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();

    for (int i = 0; i < blockData.surfaces.size(); i++) {
        const std::vector<BlockData::SurfaceData>& surfaces = blockData.surfaces[i];

        BlockData::SurfaceData surface = BlockData::SurfaceData();

        if (surfaces.size() == 0) {
            continue;

        } else if (surfaces.size() != 1) {
            int index = H2DE_RandomIntegerInRange(0, surfaces.size() - 1);
            surface = surfaces[index];

        } else {
            surface = surfaces[0];
        }

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.textureName = surface.sheet;
        sd.rect = surface.dest;
        sd.color = surface.defaultColor;
        sd.pivot = sd.rect.getSize().getCenter();
        sd.rotation = surface.rotation;

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

// EVENTS
void Block::specialEffect(const Player* player) const {
    
}

// GETTER
const std::optional<H2DE_LevelRect> Block::getHitbox() const {
    if (blockData.hitbox.has_value()) {
        return H2DE_GetObjectHitboxWorldRect(object, "main");
    }
    return std::nullopt;
}

const std::optional<int> Block::getSpeedValue() const {
    return std::nullopt;
}
