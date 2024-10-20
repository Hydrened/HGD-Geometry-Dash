#ifndef LEVEL_H
#define LEVEL_H


#include "game.h"
#include "level_loader.h"
#include "item.h"
#include "player.h"

class Game;
class Item;
class Player;

class Level {
private:
    Game* game;
    LevelData* data = nullptr;
    GameData* gameData = new GameData();
    
    H2DE_TimelineManager* topGroundTM = H2DE_CreateTimelineManager();
    H2DE_TimelineManager* botGroundTM = H2DE_CreateTimelineManager();

    int id;
    int attempts = 0;
    int speed;
    int levelLength;

    LevelMode mode = NORMAL_MODE;
    float bestNormalMode;
    float bestPracticeMode;

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

    BlockEffect currentBlockEffect = FADE;

    Player* player = nullptr;

    void initFromSave();
    void initCamera();
    void initLevelElements();
    void initItems();
    void initConfig();
    void saveData();

    void updateBackground();
    void updateBackgroundY();
    void renderElements();
    void renderPracticeUI();

    static bool sortItems(Item* item1, Item* item2);

public:
    Level(Game* game, int id);
    ~Level() noexcept(false);

    void update();
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
    LevelMode getMode() const;
    BlockEffect getBlockEffect() const;

    void setBackgroundColor(Color color);
    void setGroundColor(Color color);
    void setLineColor(Color color);
    void setTopGroundPos(LevelPos pos, unsigned int ms);
    void setBotGroundPos(LevelPos pos, unsigned int ms);
    void setMode(LevelMode mode);
    void setBestNormalMode(float percentage);
    void setBestPracticeMode(float percentage);
    void setBlockEffect(BlockEffect effect);
};

#endif
