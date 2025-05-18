#ifndef BLOCK_H
#define BLOCK_H

#include "game.h"

class Block : public Item {
private:
    H2DE_Object* object = nullptr;
    BlockData blockData;

    void initObject();

    void destroyObject();

public:
    Block(Game* game, const ItemData& itemData, const BlockData& blockData);
    ~Block() override;

    void update() override;

    void specialEffect(const Player* player) const;
    
    const std::optional<H2DE_LevelRect> getHitbox() const;
    inline const BlockType getType() const { return blockData.type; }
    const std::optional<int> getSpeedValue() const;
};

#endif
