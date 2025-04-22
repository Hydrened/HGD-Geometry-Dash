#include "items/trigger.h"

// INIT
Trigger::Trigger(Game* g, const ItemBuffer& ib, const TriggerBuffer& tb) : Item(g, ib), triggerBuffer(tb) {

}

// CLEANUP
Trigger::~Trigger() {

}

// UPDATE
void Trigger::update() {
    
}

// GETTER
bool Trigger::hasToBeRemoved(const H2DE_LevelPos& camPos, const H2DE_LevelPos& playerPos) const {
    return itemBuffer.pos.x + 1.0f < playerPos.x;
}
