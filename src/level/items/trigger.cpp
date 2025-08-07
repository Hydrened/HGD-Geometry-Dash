#include "level/items/trigger.h"

#include "level/level.h"
#include "level/scenery.h"

// INIT
Trigger::Trigger(Game* g, Level* l, const ItemData& id, const TriggerData& bd) : Item(g, l, id), triggerData(bd) {
    static const Data* gameData = game->getData();
    const Data::TriggerBuffer& buffer = gameData->getTriggerBuffer(itemData.id);

    initType(buffer);
}

void Trigger::initType(const Data::TriggerBuffer& buffer) {
    type = buffer.type;
}

// ACTIONS
void Trigger::trigger(float startBlend) {
    triggered = true;

    switch (type) {
        case TRIGGER_TYPE_BACKGROUND_COLOR:
            setColor([this](const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
                level->getScenery()->setBackgroundColor(color, duration, start);
            }, triggerData.color, triggerData.duration, startBlend);
            break;

        case TRIGGER_TYPE_GROUND_COLOR:
            setColor([this](const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
                level->getScenery()->setGroundColor(color, duration, start);
            }, triggerData.color, triggerData.duration, startBlend);
            break;

        case TRIGGER_TYPE_LINE_COLOR:
            setColor([this](const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
                level->getScenery()->setLineColor(color, duration, start);
            }, triggerData.color, triggerData.duration, startBlend);
            break;

        default: return;
    }
}

// GETTER
bool Trigger::hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const {
    return (itemData.translate.x + 2.0f < playerTranslate.x);
}

// SETTER
void Trigger::setColor(const std::function<void(H2DE_ColorRGB, uint32_t, uint32_t)>& call, const std::optional<H2DE_ColorRGB>& color, std::optional<uint32_t> duration, float startBlend) {
    if (!color.has_value() || !duration.has_value() || !call) {
        return;
    }

    uint32_t startOffset = static_cast<uint32_t>(H2DE::round(startBlend * duration.value()));
    call(color.value(), duration.value(), startOffset);
}
