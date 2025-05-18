#ifndef ITEM_H
#define ITEM_H

#include "game.h"
class Game;
class Level;

class Item {
protected:
    Game* game;
    ItemData itemData;

public:
    Item(Game* game, const ItemData& itemData);
    virtual ~Item();

    virtual void update() = 0;

    inline const H2DE_LevelPos getPos() const { return itemData.pos; }
    inline bool hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const { return (itemData.maxX < camPos.x); }
};

#include "items/block.h"
#include "items/trigger.h"

#endif
