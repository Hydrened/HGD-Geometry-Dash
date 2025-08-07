#pragma once

#include "game.h"
class Game;

class Item {
public:
    Item(Game* game, Level* level, const ItemData& itemData);
    virtual ~Item() = default;

    constexpr const H2DE_Translate& getTranslate() const {
        return itemData.translate;
    }
    virtual bool hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const = 0;

protected:
    Game* game;
    Level* level;
    ItemData itemData;
};

#include "level/items/block.h"
#include "level/items/trigger.h"
