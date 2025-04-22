#ifndef ITEM_H
#define ITEM_H

#include "game.h"
class Game;

class Item {
private:

protected:
    Game* game;
    ItemBuffer itemBuffer;

public:
    Item(Game* game, const ItemBuffer& buffer);
    virtual ~Item();

    virtual void update() = 0;

    virtual bool hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const = 0;
};

#include "items/block.h"
#include "items/trigger.h"

#endif
