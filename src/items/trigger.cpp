#include "items/trigger.h"

// INIT
Trigger::Trigger(Game* g, const ItemData& id, const TriggerData& td) : Item(g, id), triggerData(td) {

}

// CLEANUP
Trigger::~Trigger() {

}

// UPDATE
void Trigger::update() {
    
}

// ACTIONS
void Trigger::activate(Level* level, float blend) const {
    switch (triggerData.type) {
        case TRIGGER_TYPE_BACKGROUND_COLOR:
            level->setBackgroundColor(triggerData.color, triggerData.duration, blend);
            break;

        case TRIGGER_TYPE_GROUND_COLOR:
            level->setGroundColor(triggerData.color, triggerData.duration, blend);
            break;

        case TRIGGER_TYPE_LINE_COLOR:
            level->setLineColor(triggerData.color, triggerData.duration, blend);
            break;

        default: break;
    }

    level->removeTrigger(this);
}
