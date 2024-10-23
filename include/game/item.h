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
    BufferedBlock* bb;
    int currentSprite = -1;
    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();
    
    bool pickedUp = false;
    std::optional<BlockEffect> effect = std::nullopt;
    std::optional<int> coinIndex = std::nullopt;

    std::vector<BlockTextureData*> texturesData;

    Uint8 textureOpacity = SDL_MAX_UINT8;
    LevelOffset textureOffset = { 0.0f, 0.0f };
    float textureScale = 1.0f;

    void renderTexture(BlockTextureData* textureData);
    void renderGlow();
    void renderHitbox();

    LevelColor getLevelColor(BlockTextureData* bt, BlockTextureData* dt, BlockTextureData* td) const;
    int getIndex(BlockTextureData* bt, BlockTextureData* dt, BlockTextureData* td) const;

public:
    Block(Game* game, BufferedBlock* bufferedBlock);
    ~Block() override;

    void update() override;
    void render() override;
    void reset() override;

    BufferedBlock* getBufferedData() const;
    bool entered() const;
    bool isPickedUp() const;

    void enter();
    void setCoinIndex(int index);
    void setPickedUp(bool value);
};



class Trigger : public Item {
private:
    BufferedTrigger* bt;
    H2DE_TimelineManager* tm = H2DE_CreateTimelineManager();

public:
    Trigger(Game* game, BufferedTrigger* bufferedTrigger);
    ~Trigger() override;

    void update() override;
    void render() override;
    void reset() override;

    BufferedTrigger* getBufferedData() const;

    void trigger();
};



class ItemManager {
public:
    static Block* castToBlock(Item* item);
    static Trigger* castToTrigger(Item* item);
};

#endif
