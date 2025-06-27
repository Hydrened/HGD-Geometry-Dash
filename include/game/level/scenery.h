#ifndef SCENERY_H
#define SCENERY_H

#include "game.h"

class Scenery {
public:
    Scenery(Game* game, Speed speed, uint8_t backgroundID, uint8_t groundID);
    ~Scenery();

    void update();

    inline H2DE_BasicObject* getBotGround() const { return botGroundHitbox; }
    inline H2DE_BasicObject* getTopGround() const { return topGroundHitbox; }

    void setBackgroundColor(const H2DE_ColorRGB& color, uint32_t duration = 0u, uint32_t start = 0u);
    void setGroundColor(const H2DE_ColorRGB& color, uint32_t duration = 0u, uint32_t start = 0u);
    void setLineColor(const H2DE_ColorRGB& color, uint32_t duration = 0u, uint32_t start = 0u);

private:
    Game* game;
    Speed speed;
    uint8_t backgroundID;
    uint8_t groundID;

    enum SceneryType {
        SCENERY_TYPE_BACKGROUND,
        SCENERY_TYPE_GROUND,
        SCENERY_TYPE_LINE,
    };

    H2DE_BasicObject* displayedBackground = nullptr;
    H2DE_BasicObject* displayedBotGround = nullptr;
    H2DE_BasicObject* displayedTopGround = nullptr;

    H2DE_BasicObject* botGroundHitbox = nullptr;
    H2DE_BasicObject* topGroundHitbox = nullptr;

    float displayedBackgroundTranslateY = 0.0f;
    float collisionBotGroundTranslateY = 0.0f;
    float collisionTopGroundTranslateY = 0.0f;
    float displayedBotGroundTranslateY = 0.0f;
    float displayedTopGroundTranslateY = 0.0f;

    H2DE_ColorRGB backgroundColor = { 0, 0, 0, 0 };
    H2DE_ColorRGB groundColor = { 0, 0, 0, 0 };
    H2DE_ColorRGB lineColor = { 0, 0, 0, 0 };

    H2DE_TimelineID backgroundColorTimelineID = H2DE_INVALID_TIMELINE_ID;
    H2DE_TimelineID botGroundColorTimelineID = H2DE_INVALID_TIMELINE_ID;
    H2DE_TimelineID topGroundColorTimelineID = H2DE_INVALID_TIMELINE_ID;
    H2DE_TimelineID lineColorTimelineID = H2DE_INVALID_TIMELINE_ID;

    void initDefaultValues();
    void initBackground();
    void initGrounds();
    H2DE_BasicObject* createDisplayedGround(const H2DE_ObjectData& objectData) const;
    H2DE_BasicObject* createGroundHitbox(const H2DE_ObjectData& objectData) const;

    void destroyBackground();
    void destroyGrounds();

    void updateTranslations();
    void updateBackroundObjectTranslations();
    void updateGroundObjectsTranslations();
    void updateBackroundTilesTranslations();
    void updateGroundTilesTranslations();

    void addMissingTiles();
    void addMissingTiles(H2DE_BasicObject* object, const H2DE_Scale& tileScale, SceneryType type);

    void createTile(H2DE_BasicObject* object, float translateX, SceneryType type);

    void removeOutOfScreenTiles();
    void removeOutOfScreenTiles(H2DE_BasicObject* object, SceneryType type);

    void stopTimeline(H2DE_TimelineID& id);

    inline std::array<H2DE_BasicObject*, 4> getGroundsObjects() const { return { displayedBotGround, displayedTopGround, botGroundHitbox, topGroundHitbox }; }
    inline std::array<H2DE_BasicObject*, 2> getGroundDisplayedObjects() const { return { displayedBotGround, displayedTopGround }; }
    std::vector<H2DE_Texture*> getTiles(const H2DE_BasicObject* object) const;
    std::array<H2DE_Texture*, 2> getLines() const;

    inline bool isSurfaceTile(const std::string& name) const { return (name.find("tile_") != std::string::npos); }
    float getLastTileTranslateX(const H2DE_BasicObject* object, SceneryType type) const;

    void setColor(const H2DE_BasicObject* object, const H2DE_ColorRGB& color, SceneryType type);
    H2DE_TimelineID setColor(const H2DE_BasicObject* object, const H2DE_ColorRGB& defaultColor, const H2DE_ColorRGB& color, uint32_t duration, uint32_t start, SceneryType type);
};

#endif
