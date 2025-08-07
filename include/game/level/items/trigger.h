#pragma once

#include "level/items/item.h"
class Game;

class Trigger : public Item {
public:
    Trigger(Game* game, Level* level, const ItemData& itemData, const TriggerData& triggerData);
    ~Trigger() = default;

    void trigger(float startBlend = 0.0f);

    constexpr const std::optional<uint32_t> getDuration() const {
        return triggerData.duration;
    }
    constexpr bool isTouchTrigger() const {
        return triggerData.touchTrigger;
    }
    constexpr bool isTriggered() const {
        return triggered;
    }
    bool hasToBeRemoved(const H2DE_LevelRect& camRect, const H2DE_Translate& playerTranslate) const override;

private:
    TriggerData triggerData;
    TriggerType type = TRIGGER_TYPE_BACKGROUND_COLOR;

    bool triggered = false;

    void initType(const Data::TriggerBuffer& buffer);
    
    void setColor(const std::function<void(H2DE_ColorRGB, uint32_t, uint32_t)>& call, const std::optional<H2DE_ColorRGB>& color, std::optional<uint32_t> duration, float startBlend);
};
