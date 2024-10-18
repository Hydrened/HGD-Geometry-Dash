#ifndef ITEM_H
#define ITEM_H

#include "game.h"

class Item {
protected:
    Game* game;
    bool used;

public:
    Item(Game* game);
    virtual ~Item();

    virtual void update() = 0;
    virtual void render() = 0;
    virtual void reset() = 0;

    void setUsed();
};



class Block : public Item {
private:
    BufferedBlock* data;
    int currentSprite = -1;
    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();
    
    bool pickedUp = false;
    std::optional<BlockEffect> effect = std::nullopt;

    Uint8 textureOpacity = SDL_MAX_UINT8;
    LevelOffset textureOffset = { 0.0f, 0.0f };
    float textureScale = 1.0f;

    void renderTexture();
    void renderGlow();
    void renderHitbox();

public:
    Block(Game* game, BufferedBlock* data);
    ~Block() override;

    void update() override;
    void render() override;
    void reset() override;

    BufferedBlock* getData() const;
    bool entered() const;
    bool isPickedUp() const;

    void enter();
};



class Trigger : public Item {
private:
    BufferedTrigger* data;
    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

public:
    Trigger(Game* game, BufferedTrigger* data);
    ~Trigger() override;

    void update() override;
    void render() override;
    void reset() override;

    BufferedTrigger* getData() const;

    void trigger();
};



class ItemManager {
public:
    static Block* castToBlock(Item* item);
    static Trigger* castToTrigger(Item* item);
};

#endif
