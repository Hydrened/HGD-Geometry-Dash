#ifndef LEVEL_H
#define LEVEL_H

#include "game.h"
#include "utils.h"
class Game;
class Scenery;
class Player;

class Level {
public:
    Level(Game* game, Level_ID id);
    Level(Game* game, Level_ID id, const Checkpoint& checkpoint);
    ~Level();
    
    void close(const std::function<void()>& callback);
    void newAttempt();
    
    void update();

    inline Player* getPlayer() const { return player; }

private:
    struct BlockBuffer {
        std::string id = "";
        H2DE_Translate translate = { 0.0f, 0.0f };
        float rotation = 0.0f;
        BlockFlip flip = BLOCK_FLIP_NONE;
    };

    struct TriggerBuffer {
        std::string id = "";
        H2DE_Translate translate = { 0.0f, 0.0f };
        std::optional<H2DE_ColorRGB> color = std::nullopt;
        std::optional<uint32_t> duration = 0;
        bool touchTrigger = false;
    };

private:
    Game* game;
    Level_ID id;
    Checkpoint checkpoint;

    json data;
    uint32_t attempts = 0;
    Scenery* scenery = nullptr;
    Player* player = nullptr;

    std::vector<BlockBuffer> blockBuffers = {};
    std::vector<TriggerBuffer> triggerBuffers = {};

    void init();
    void initData();
    void initCheckpoint();
    void initScenery();
    void initCamera();
    void initItemBuffers();
    void initBlockBuffers();
    void initTriggerBuffers();
    void initPlayer();

    void destroyScenery();
    void destroyPlayer();
};

#endif
