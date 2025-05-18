#ifndef SCENERY_H
#define SCENERY_H

#include "game.h"
class Game;

class Scenery {
private:
    Game* game;
    int backgroundID;
    int groundID;

    enum SceneryType {
        BACKGROUND,
        BOT_GROUND,
        TOP_GROUND,
    };

    H2DE_Object* botGround = nullptr;
    H2DE_Object* topGround = nullptr;

    std::vector<H2DE_Object*> backgroundTiles = {};
    std::vector<H2DE_Object*> botGroundTiles = {};
    std::vector<H2DE_Object*> topGroundTiles = {};
    std::vector<H2DE_Object*> lines = {};

    float displayedBackgroundPosY = 0.0f;
    float collisionBotGroundPosY = 0.0f;
    float collisionTopGroundPosY = 0.0f;
    float displayedBotGroundPosY = 0.0f;
    float displayedTopGroundPosY = 0.0f;

    H2DE_ColorRGB backgroundColor = { 0, 0, 0, 0 };
    H2DE_ColorRGB groundColor = { 0, 0, 0, 0 };
    H2DE_ColorRGB lineColor = { 0, 0, 0, 0 };

    std::optional<unsigned int> backgroundColorTimlineID = std::nullopt;
    std::optional<unsigned int> botGroundColorTimlineID = std::nullopt;
    std::optional<unsigned int> topGroundColorTimlineID = std::nullopt;
    std::optional<unsigned int> lineColorTimlineID = std::nullopt;

    void initDefaultValues();
    void createGrounds();
    H2DE_Object* createGround(SceneryType type);
    void createLines();
    H2DE_Object* createLine(SceneryType type);

    void destroyGrounds();
    void destroyTiles();
    void destroyLines();
    void stopTimelines();
    void stopTimeline(std::optional<unsigned int>& id);

    void updatePositions();
    void updateBackgroundTilesPosition();
    void updateGroundTilesPosition();
    void updateGroundTilesPosition(const std::vector<H2DE_Object*>& tiles, float posY);
    void updateGroundsPositions();
    void updateLinesPosition();

    void removeOutOfScreenTiles();
    void removeOutOfScreenTiles(std::vector<H2DE_Object*>& tiles, const H2DE_LevelSize& tileSize);

    void addMissingTiles();
    void addMissingTiles(std::vector<H2DE_Object*>& tiles, const H2DE_LevelPos& tileDefaultPos, const H2DE_LevelSize& tileSize, SceneryType type);

    void createTile(float posX, SceneryType type);
    void createBackgroundTile(float posX);
    void createGroundTile(float posX, SceneryType type);

    void setColors(const std::vector<H2DE_Object*>& tiles, const H2DE_ColorRGB& color);
    unsigned int setColors(const std::vector<H2DE_Object*>& tiles, H2DE_ColorRGB& defaultColor, const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend);

public:
    Scenery(Game* game, int backgroundID, int groundID);
    ~Scenery();

    void update();
    
    void reset();

    inline const H2DE_Object* getBotGround() const { return botGround; }
    inline const H2DE_Object* getTopGround() const { return topGround; }

    void setBackgroundColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend = 0.0f);
    void setBackgroundColor(const H2DE_ColorRGB& color, float defaultBlend = 0.0f);
    void setGroundColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend = 0.0f);
    void setGroundColor(const H2DE_ColorRGB& color, float defaultBlend = 0.0f);
    void setLineColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend = 0.0f);
    void setLineColor(const H2DE_ColorRGB& color, float defaultBlend = 0.0f);
};

#endif
