#ifndef BLOCK_H
#define BLOCK_H

#include "game.h"

class Block : public Item {
private:
    BlockBuffer blockBuffer;
    H2DE_Object* object = nullptr;
    BlockData data;

    void loadData();
    void initObject();

    void destroyObject();

public:
    Block(Game* game, const ItemBuffer& itemBuffer, const BlockBuffer& buffer);
    ~Block() override;

    void update() override;
    
    bool hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const override;
};

#endif
