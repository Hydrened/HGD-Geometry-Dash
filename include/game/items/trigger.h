#ifndef TRIGGER_H
#define TRIGGER_H

#include "game.h"
class Level;

class Trigger : public Item {
private:
    TriggerData triggerData;

public:
    Trigger(Game* game, const ItemData& itemData, const TriggerData& trigerData);
    ~Trigger() override;

    void update() override;

    void activate(Level* level, float blend = 0.0f) const;

    inline int getDuration() const { return triggerData.duration; }
    inline bool getTouchTrigger() const { return triggerData.touchTrigger; }
};

#endif
