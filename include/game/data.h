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
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, std::pair<H2DE_LevelRect, H2DE_LevelRect>>> playerHitboxes = {};
    std::unordered_map<MenuID, std::vector<ObjectBuffer>> menuObjects = {};
    std::unordered_map<int, IconData> cubes = {};
    std::unordered_map<std::string, BlockData> blocks = {};

    std::vector<float> speeds = { 0.0f, 0.1723f, 0.0f, 0.0f, 0.0f };

    H2DE_LevelPos defaultCameraPos = { -6.5f, -3.0f };
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

    template<typename T_Physic>
    static const T_Physic& getPhysic(const std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, T_Physic>>& physics, PlayerGamemode gamemode, PlayerSize size);
    
    void loadBlocksData();

    void initPhysics();
    void initGravities();
    void initMaxGravities();
    void initRotations();
    void initClicks();
    void initPlayerHiboxes();

    void initMenuObjects();
    void initMainMenuObjects();
    void initLevelMenuObjects();

    void initIcons();
    void initCubes();

public:
    Data(Game* game);
    ~Data();

    const float& getGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getMaxGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getRotation(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getClick(PlayerGamemode gamemode, PlayerSize size) const;
    const std::pair<H2DE_LevelRect, H2DE_LevelRect>& getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const;
    const std::vector<ObjectBuffer>& getMenuObjects(MenuID id) const;
    const IconData& getCube(int id) const;
    const BlockData& getBlock(const std::string& id) const;

    const float& getSpeed(int speed) const;
    const int getSpeed(float vx) const;

    const H2DE_LevelPos& getDefaultCameraPos() const;
    const H2DE_LevelPos& getDefaultBackgroundPos() const;
    const H2DE_LevelPos& getDefaultBotGroundPos() const;
    const H2DE_LevelPos& getDefaultTopGroundPos() const;

    const H2DE_LevelSize& getGameSize() const;
    const H2DE_LevelSize& getBackgroundTileSize() const;
    const H2DE_LevelSize& getGroundTileSize() const;
    const H2DE_LevelSize& getGroundSize() const;
    const H2DE_LevelSize& getLineSize() const;

    const H2DE_ColorRGB& getDefaultBackgroundColor() const;
    const H2DE_ColorRGB& getDefaultGroundColor() const;
    const H2DE_ColorRGB& getDefaultLineColor() const;

    const int& getGroundMovementDuration() const;
    const float& getLineOffsetX() const;
};

#endif
