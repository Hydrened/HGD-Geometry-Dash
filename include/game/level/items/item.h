#ifndef ITEM_H
#define ITEM_H

#include "game.h"
class Game;

class Item {
public:
    Item(Game* game, Level* level, const ItemData& itemData);
    virtual ~Item() = default;

    virtual bool hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const = 0;

protected:
    Game* game;
    Level* level;
    ItemData itemData;

private:

};

#include "level/items/block.h"
#include "level/items/trigger.h"

#endif
