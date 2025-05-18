#ifndef DATA_H
#define DATA_H

#include <stdexcept>
#include <unordered_map>
#include "game.h"
#include "utils.h"
class Game;

class Data {
private:
    Game* game;
    H2DE_Engine* engine = nullptr;

    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> gravities = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> maxGravities = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> clicks = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> rotations = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, PlayerHitbox>> playerHitboxes = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, PlayerSnap>> playerSnaps = {};
    std::unordered_map<MenuID, std::vector<ObjectBuffer>> menuObjects = {};
    std::unordered_map<int, IconData> cubes = {};
    std::unordered_map<std::string, BlockData> blockDatas = {};
    std::unordered_map<std::string, TriggerData> triggerDatas = {};
    std::unordered_map<std::string, H2DE_ColorRGB> hitboxesColors = {};

    Checkpoint defaultCheckpoint = {};

    std::vector<float> speeds = { 0.0f, 0.1723f, 0.0f, 0.0f, 0.0f };

    H2DE_LevelPos defaultCameraPos = { -6.5f, -3.0f };
    H2DE_LevelPos startposCameraOffset = { -6.5f, -8.0f };
    H2DE_LevelPos defaultBackgroundPos = { -6.5f, -4.0f };
    H2DE_LevelPos defaultBotGroundPos = { -6.5f, -4.0f };
    H2DE_LevelPos defaultTopGroundPos = { -6.5f, 100.0f };

    H2DE_LevelSize gameSize = { 19.0f, 10.6875f };
    H2DE_LevelSize backgroundTileSize = { 19.0f, 19.0f };
    H2DE_LevelSize groundTileSize = { 4.0f, 4.0f };
    H2DE_LevelSize groundSize = { 19.0f, 4.0f };
    H2DE_LevelSize lineSize = { 17.0f, 0.05f };

    H2DE_ColorRGB defaultBackgroundColor = { 10, 97, 255, 255 };
    H2DE_ColorRGB defaultGroundColor = { 8, 97, 252, 255 };
    H2DE_ColorRGB defaultLineColor = { 255, 255, 255, 255 };

    int groundMovementDuration = 800;
    float lineOffsetX = 1.5f;
    float minCamY = -3.0f;
    float maxCamY = 70.0f;
    float cameraBlockPadding = 3.0f;
    float backgroundRatio = 0.9f;
    float triggerSize = 1.0f;

    template<typename T_Physic>
    static const T_Physic& getPhysic(const std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, T_Physic>>& physics, PlayerGamemode gamemode, PlayerSize size);
    
    void loadItemsData();
    void loadBlocksData();
    void loadTriggersData();
    static const std::vector<std::vector<BlockData::SurfaceData>> getBlockSurfacesDatas(const json& block, const std::string& id);

    void initPhysics();
    void initGravities();
    void initMaxGravities();
    void initRotations();
    void initClicks();
    void initPlayerHiboxes();
    void initPlayerSnaps();

    void initMenuObjects();
    void initMainMenuObjects();
    void initLevelMenuObjects();

    void initIcons();
    void initCubes();

    void initHitboxesColors();

    void initDefaultCheckpoint();

    static inline float snapRotation(float value, float deg) { return std::round(value / deg) * deg; }
    template<typename H2DE_Rect_T>
    static H2DE_Rect<H2DE_Rect_T> rotateRect(const H2DE_Rect<H2DE_Rect_T>& rect, float rotation);

public:
    Data(Game* game);
    ~Data();

    const float& getGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getMaxGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getRotation(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getClick(PlayerGamemode gamemode, PlayerSize size) const;
    const PlayerHitbox& getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const;
    const PlayerSnap& getPlayerSnap(PlayerGamemode gamemode, PlayerSize size) const;
    const std::vector<ObjectBuffer>& getMenuObjects(MenuID id) const;
    const IconData& getCube(int id) const;
    const BlockData getBlockData(const std::string& id, const json& block) const;
    const TriggerData getTriggerData(const std::string& id, const json& trigger) const;

    const float& getSpeed(int speed) const;
    const int getSpeed(float vx) const;
    const H2DE_ColorRGB& getHitboxColor(const std::string& color) const;
    const H2DE_ColorRGB& getHitboxColor(BlockType blockType) const;
    inline const Checkpoint& getDefaultCheckpoint() const { return defaultCheckpoint; }

    inline const H2DE_LevelPos& getDefaultCameraPos() const { return defaultCameraPos; }
    inline const H2DE_LevelPos& getStartposCameraOffset() const { return startposCameraOffset; }
    inline const H2DE_LevelPos& getDefaultBackgroundPos() const { return defaultBackgroundPos; }
    inline const H2DE_LevelPos& getDefaultBotGroundPos() const { return defaultBotGroundPos; }
    inline const H2DE_LevelPos& getDefaultTopGroundPos() const { return defaultTopGroundPos; }

    inline const H2DE_LevelSize& getGameSize() const { return gameSize; }
    inline const H2DE_LevelSize& getBackgroundTileSize() const { return backgroundTileSize; }
    inline const H2DE_LevelSize& getGroundTileSize() const { return groundTileSize; }
    inline const H2DE_LevelSize& getGroundSize() const { return groundSize; }
    inline const H2DE_LevelSize& getLineSize() const { return lineSize; }

    inline const H2DE_ColorRGB& getDefaultBackgroundColor() const { return defaultBackgroundColor; }
    inline const H2DE_ColorRGB& getDefaultGroundColor() const { return defaultGroundColor; }
    inline const H2DE_ColorRGB& getDefaultLineColor() const { return defaultLineColor; }

    inline const int& getGroundMovementDuration() const { return groundMovementDuration; }
    inline const float& getLineOffsetX() const { return lineOffsetX; }
    inline const float& getMinCamY() const { return minCamY; }
    inline const float& getMaxCamY() const { return maxCamY; }
    inline const float& getCameraBlockPadding() const { return cameraBlockPadding; }
    inline const float& getBackgroundRatio() const { return backgroundRatio; }
    inline const float& getTriggerSize() const { return triggerSize; }
};

#endif
