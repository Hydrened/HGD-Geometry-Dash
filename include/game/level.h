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
    const Checkpoint* checkpoint;

    json data;
    std::vector<std::pair<ItemBuffer, std::pair<std::optional<BlockBuffer>, std::optional<TriggerBuffer>>>> itemBuffer = {};
    int itemBufferIndex = 0;
    int startDelayID = -1;
    
    Scenery* scenery = nullptr;
    Player* player = nullptr;
    std::vector<Item*> items = {};

    int speed = -1;

    void loadData();
    void initItemBuffer();
    void initItemBlockBuffer();
    void initItemTriggerkBuffer();
    void initCamera() const;
    void initScenery();
    void initPlayer();
    void initSpeed();
    void initStartDelay();
    void playSong() const;

    void stopStartDelay();
    void destroyScenery();
    void destroyPlayer();
    void destroyItems();

    void updateCamera();
    void updateScenery();
    void updatePlayer();
    void updateItemBuffer();
    void updateItems();

    static const ItemBuffer getItemBuffer(const json& item);

public:
    Level(Game* game, int id, const Checkpoint* checkpoint);
    ~Level();

    void update();

    Player* getPlayer() const;
    const int getSpeed() const;

    void setSpeed(int speed);
};

#endif
