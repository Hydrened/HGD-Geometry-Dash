#ifndef TRIGGER_H
#define TRIGGER_H

#include "level/items/item.h"
class Game;

class Trigger : public Item {
public:
    Trigger(Game* game, Level* level, const ItemData& itemData, const TriggerData& triggerData);
    ~Trigger() = default;

    bool hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const override;

private:
    TriggerData triggerData;
};

#endif
