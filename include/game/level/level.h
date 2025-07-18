#ifndef LEVEL_H
#define LEVEL_H

#include "game.h"
#include "utils.h"
class Game;
class Scenery;
class Player;
class Item;
class Block;
class Trigger;

class Level {
private:
    struct BlockBuffer {
        ItemData itemData = ItemData();
        BlockData blockData = BlockData();
    };

    struct TriggerBuffer {
        ItemData itemData = ItemData();
        TriggerData triggerData = TriggerData();
    };

public:
    Level(Game* game, Level_ID id);
    Level(Game* game, Level_ID id, const Checkpoint& checkpoint);
    ~Level();

    void close(const std::function<void()>& callback);
    
    void newAttempt();
    void playerDied();
    
    void update();

    inline Player* getPlayer() const { return player; }
    inline const std::vector<Block*>& getBlocks() const { return blocks; }
    inline const std::vector<Trigger*>& getTriggers() const { return triggers; }

private:
    Game* game;
    Level_ID id;
    Checkpoint checkpoint;

    json data;
    uint32_t attempts = 0;
    Scenery* scenery = nullptr;
    Player* player = nullptr;

    std::vector<Item*> items = {};
    std::vector<Block*> blocks = {};
    std::vector<Trigger*> triggers = {};

    std::vector<Level::BlockBuffer> blocksBuffer = {};
    std::vector<Level::TriggerBuffer> triggersBuffer = {};
    uint32_t blockBufferIndex = 0;
    uint32_t triggerBufferIndex = 0;

    H2DE_DelayID startingDelayID = H2DE_INVALID_DELAY_ID;
    H2DE_DelayID respawningDelayID = H2DE_INVALID_DELAY_ID;

    void init();
    void initData();
    void initCheckpoint();
    void initCamera();
    void initScenery();
    void initItemBuffers();
    void initBlockBuffers();
    void initTriggerBuffers();
    void initItems();
    void initPlayer();
    void initStartingDelay();

    void destroyItems();
    void destroyItem(Item* item);
    void destroyScenery();
    void destroyPlayer();
    void destroyStartingDelay();
    void destroyRespawningDelay();
    void stopSong();
    void playCloseSfx();

    void updatePlayer();
    void updateItemsBuffers();
    void updateBlocksBuffer();
    void updateTriggersBuffer();
    void updateItemVector();
    void updateCamera();
    void updateScenery();
};

#endif
