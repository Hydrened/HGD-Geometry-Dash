#ifndef LEVEL_H
#define LEVEL_H

#define BLOCKS_ON_WIDTH 19

class Game;
class Item;
class Player;

#include <vector>
#include "game.h"
#include "item.h"
#include "player.h"
#include "data.h"
#include "calculator.h"

class Level {
private:
    Game* game;
    LevelData* data = nullptr;
    GameData* gameData = new GameData();

    int id;
    int attempts;
    int speed;
    int levelLength;

    LevelPos backgroundPos;
    LevelPos botGroundPos;
    LevelPos topGroundPos;
    Color backgroundColor;
    Color groundColor;
    Color lineColor;

    std::vector<Item*> items;

    Player* player = nullptr;

    class Levels {
    private:
        static void getMenuInfos(LevelData* data, json* level);
        static void getConfigInfos(LevelData* data, json* level);
        static void getBlocksInfos(LevelData* data, json* level, json* itemsData);
        static void getTriggersInfos(LevelData* data, json* level, json* itemsData);
        static void getStartposInfo(LevelData* data, json* level, json* itemsData);

    public:
        static LevelData* getLevelData(int id);
        static void loadLevelData(int id, const fs::path& gmdFile);
    };

    void initAttempts();
    void initCamera();
    void initLevelElements();
    void initItems();
    void initConfig();

    static bool sortItems(Item* item1, Item* item2);

public:
    Level(Game* game, int id);
    ~Level();

    void update();
    void render();

    void finish();
    void pause();
    void resume();

    LevelData* getData();
    int getCurrentSpeed();
    LevelPos getBotGroundPos();
    LevelPos getTopGroundPos();
    Player* getPlayer();
    std::vector<Item*>* getItems();
    int getLevelLength();
};

#endif
