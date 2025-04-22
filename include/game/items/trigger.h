#ifndef TRIGGER_H
#define TRIGGER_H

#include "game.h"

class Trigger : public Item {
private:
    TriggerBuffer triggerBuffer;

public:
    Trigger(Game* game, const ItemBuffer& itemBuffer, const TriggerBuffer& buffer);
    ~Trigger() override;

    void update() override;

    bool hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const override;
};

#endif
