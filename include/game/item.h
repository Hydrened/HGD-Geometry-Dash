#ifndef ITEM_H
#define ITEM_H

#include "game.h"

class Item {
protected:
    Game* game;
    bool used;
    H2DE_TimelineManager* tm = new H2DE_TimelineManager();

public:
    Item(Game* game);
    virtual ~Item();

    virtual void update() = 0;
    virtual void render() = 0;
    virtual void reset() = 0;
};



class Block : public Item {
private:
    BufferedBlock* data;

public:
    Block(Game* game, BufferedBlock* data);
    ~Block() override;

    void update() override;
    void render() override;
    void renderTexture();
    void renderHitbox();
    void reset() override;

    BufferedBlock* getData() const;
    bool entered() const;

    void enter();
};



class Trigger : public Item {
private:
    BufferedTrigger* data;

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
