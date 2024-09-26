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

    bool isUsed();
    void setUsed();
};



class Block : public Item {
private:
    BufferedBlock* data;

public:
    Block(Game* game, BufferedBlock* data);
    ~Block() override;

    void update() override;
    void render() override;

    BufferedBlock* getData();
};



class Trigger : public Item {
private:
    BufferedTrigger* data;

public:
    Trigger(Game* game, BufferedTrigger* data);
    ~Trigger() override;

    void update() override;
    void render() override;

    BufferedTrigger* getData();
};



class ItemManager {
public:
    static Block* castToBlock(Item* item);
    static Trigger* castToTrigger(Item* item);
};

#endif
