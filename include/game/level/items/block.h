#ifndef BLOCK_H
#define BLOCK_H

#include "level/items/item.h"
class Game;

class Block : public Item {
public:
    Block(Game* game, Level* level, const ItemData& itemData, const BlockData& blockData);
    ~Block() override;

    constexpr const BlockType& getType() const { return type; }
    constexpr const H2DE_LevelRect& getHitboxWorldRect() const { return hitboxRect; }
    bool hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const override;

private:
    BlockData blockData;
    H2DE_BasicObject* object = nullptr;

    BlockType type = BLOCK_TYPE_DECORATION;
    H2DE_LevelRect hitboxRect = { 0.0f, 0.0f, 0.0f, 0.0f };

    void initObject(const Data::BlockBuffer& buffer);
    void initType(const Data::BlockBuffer& buffer);
    void initHitboxRect(const Data::BlockBuffer& buffer);

    void destroyObject();
};

#endif
