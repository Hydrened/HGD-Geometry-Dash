#ifndef LEVEL_H
#define LEVEL_H

#define BLOCKS_ON_WIDTH 19

class Game;
class Item;
class Player;

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
    
    H2DE_TimelineManager* topGroundTM = new H2DE_TimelineManager();
    H2DE_TimelineManager* botGroundTM = new H2DE_TimelineManager();

    int id;
    int attempts = 0;
    int speed;
    int levelLength;
    LevelMode mode = NORMAL_MODE;

    LevelPos backgroundPos;
    LevelPos botGroundPos;
    LevelPos botGroundVisualPos;
    LevelPos topGroundPos;
    LevelPos topGroundVisualPos;
    Color backgroundColor;
    Color groundColor;
    Color lineColor;

    std::vector<Item*> items;
    std::vector<bool> coins;

    Player* player = nullptr;

    void initFromSave();
    void initCamera();
    void initLevelElements();
    void initItems();
    void initConfig();
    void saveData();

    static bool sortItems(Item* item1, Item* item2);

public:
    Level(Game* game, int id);
    ~Level() noexcept(false);

    void update();
    void updateBackground();
    void render();

    void finish();
    void pause();
    void resume();
    void respawn();

    LevelData* getData() const;
    LevelPos getBotGroundPos() const;
    LevelPos getTopGroundPos() const;
    Color getBackgroundColor() const;
    Color getGroundColor() const;
    Color getLineColor() const;
    Player* getPlayer() const;
    std::vector<Item*>* getItems();
    int getCurrentSpeed() const;
    int getLevelLength() const;

    void setBackgroundColor(Color color);
    void setGroundColor(Color color);
    void setLineColor(Color color);
    void setTopGroundPos(LevelPos pos, unsigned int ms);
    void setBotGroundPos(LevelPos pos, unsigned int ms);
};

#endif
