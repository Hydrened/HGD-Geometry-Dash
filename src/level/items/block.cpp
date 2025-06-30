#include "level/items/block.h"

// INIT
Block::Block(Game* g, Level* l, const ItemData& id, const BlockData& bd) : Item(g, l, id), blockData(bd) {
    static const Data* gameData = game->getData();
    const Data::BlockBuffer& buffer = gameData->getBlockBuffer(itemData.id);

    initObject(buffer);
    initType(buffer);
    initHitboxRect(buffer);
}

void Block::initObject(const Data::BlockBuffer& buffer) {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_Scale& blockScale = game->getData()->getBlockScale();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = itemData.translate;
    od.transform.scale = { blockScale.x * blockData.flip.x, blockScale.y * blockData.flip.y };
    od.transform.rotation = blockData.rotation;
    od.index = buffer.index;

    object = engine->createObject<H2DE_BasicObject>(od);

    for (int i = 0; i < buffer.surfaces.normals.size(); i++) {
        const Data::SurfaceBuffer& surfaceBuffer = buffer.surfaces.normals[i];
        object->addSurface<H2DE_Texture>("normal" + std::to_string(i), surfaceBuffer.surface, surfaceBuffer.texture);
    }

    for (const auto& [id, surfaces] : buffer.surfaces.randoms) {
        const Data::SurfaceBuffer& randomSurfaceBuffer = H2DE::randomValueFromVector(surfaces);
        object->addSurface<H2DE_Texture>("random-" + std::to_string(id), randomSurfaceBuffer.surface, randomSurfaceBuffer.texture);
    }

    if (buffer.hitbox.has_value()) {
        object->addHitbox("main", buffer.hitbox.value());
    }
}

void Block::initType(const Data::BlockBuffer& buffer) {
    type = buffer.type;
}

void Block::initHitboxRect(const Data::BlockBuffer& buffer) {
    if (buffer.hitbox.has_value()) {
        const H2DE_Hitbox hitboxValue = buffer.hitbox.value();
        hitboxRect = hitboxValue.transform.translate.makeRect(hitboxValue.transform.scale).addTranslate(itemData.translate);
    }
}

// CLEANUP
Block::~Block() {
    destroyObject();
}

void Block::destroyObject() {
    static H2DE_Engine* engine = game->getEngine();
    engine->destroyObject(object);
}

// GETTER
bool Block::hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const {
    static const float& cameraItemPadding = game->getData()->getCameraItemPadding();
    return (itemData.translate.x + cameraItemPadding < camRect.getMinX());
}
