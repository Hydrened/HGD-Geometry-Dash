#include "level/items/trigger.h"

// INIT
Trigger::Trigger(Game* g, Level* l, const ItemData& id, const TriggerData& bd) : Item(g, l, id), triggerData(bd) {

}

// GETTER
bool Trigger::hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const {
    return (itemData.translate.x < playerTranslate.x);
}
