#ifndef LEVEL_H
#define LEVEL_H

#include "game.h"
#include "scenery.h"
#include "player.h"
#include "items/item.h"
class Game;
class Scenery;
class Player;
class Item;

class Level {
private:
    Game* game;
    int id;
    Checkpoint checkpoint;

    json data;
    std::vector<ItemData> itemBuffer = {};
    int itemBufferIndex = 0;
    int startDelayID = -1;
    int respawnDelayID = -1;
    
    Scenery* scenery = nullptr;
    Player* player = nullptr;
    std::vector<Item*> items = {};

    int speed = -1;

    bool isValid = true;

    void init();
    void loadData();
    void initItemBuffer();
    void initItemBlockBuffer();
    void initItemTriggerBuffer();
    void initCamera() const;
    void initScenery();
    void initPlayer();
    void initItems();
    void initSpeed();
    void initStartDelay();
    void playSong() const;

    void stopStartDelay();
    void stopRespawnDelay();
    void destroyScenery();
    void destroyPlayer();
    void destroyItems();

    void updatePlayer();
    void updateCamera();
    void updateScenery();
    void updateItemBuffer();
    void updateItems();

    void reset();
    void resetScenery();
    void simulateTriggers(float poxX);

    static const ItemData getItemData(const json& item);
    const int getPosTime(float posX) const;

public:
    Level(Game* game, int id);
    Level(Game* game, int id, const Checkpoint& checkpoint);
    ~Level();

    void update();

    void respawn();
    void removeTrigger(const Trigger* trigger);

    inline Player* getPlayer() const { return player; }
    inline const int getSpeed() const { return speed; }
    inline const std::vector<Item*>& getItems() const { return items; }
    const H2DE_Object* getBotGround() const;
    const H2DE_Object* getTopGround() const;

    void setSpeed(int speed);
    void setBackgroundColor(const H2DE_ColorRGB& color, int duration, float blend);
    void setGroundColor(const H2DE_ColorRGB& color, int duration, float blend);
    void setLineColor(const H2DE_ColorRGB& color, int duration, float blend);
    void playerDied();
};

#endif
