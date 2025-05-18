#ifndef UTILS_H
#define UTILS_H

#undef max

#include <limits>
#include "game.h"

enum MenuID {
    MAIN_MENU = 1,
    LEVEL_MENU = 2,
};

enum ObjectType {
    BAR_OBJECT,
    BASIC_OBJECT,
    BUTTON_OBJECT,
    TEXT_OBJECT,
};

enum Layer {
    LAYER_BG = 0,
    LAYER_B5 = 1,
    LAYER_B4 = 2,
    LAYER_B3 = 3,
    LAYER_B2 = 4,
    LAYER_B1 = 5,
    LAYER_P = 6,
    LAYER_T1 = 7,
    LAYER_T2 = 8,
    LAYER_T3 = 9,
    LAYER_T4 = 10,
    LAYER_G = 11,
    LAYER_H = 12,
    LAYER_UI = 13,
};

enum GameState {
    LOADING_ASSETS,
    MENU,
    LEVEL,
};

enum PlayerGamemode {
    PLAYER_GAMEMODE_CUBE,
    PLAYER_GAMEMODE_SHIP,
};

enum PlayerSize {
    PLAYER_SIZE_NORMAL,
    PLAYER_SIZE_MINI,
};

enum PlayerGravity {
    PLAYER_GRAVITY_NORMAL =         1,
    PLAYER_GRAVITY_UPSIDE_DOWN =   -1,
};

enum BlockType {
    BLOCK_TYPE_SOLID =                      0x00,
    BLOCK_TYPE_OBSTACLE =                   0x01,
    BLOCK_TYPE_SPECIAL =                    0x02,
    BLOCK_TYPE_DECORATION =                 0x03,
};

enum TriggerType {
    TRIGGER_TYPE_STARTPOS =                 1,
    TRIGGER_TYPE_BACKGROUND_COLOR =         2,
    TRIGGER_TYPE_GROUND_COLOR =             3,
    TRIGGER_TYPE_LINE_COLOR =               4,
    TRIGGER_TYPE_BE_FADE =                  108,
    TRIGGER_TYPE_BE_FROM_TOP =              109,
    TRIGGER_TYPE_BE_FROM_BOTTOM =           110,
    TRIGGER_TYPE_BE_FROM_LEFT =             111,
    TRIGGER_TYPE_BE_FROM_RIGHT =            112,
    TRIGGER_TYPE_BE_FROM_SCALE =            113,
};

enum LevelMode {
    LEVEL_MODE_NORMAL,
    LEVEL_MODE_PRACTICE,
};

struct PlayerHitbox {
    H2DE_LevelRect red = { 0.0f, 0.0f, 0.0f, 0.0f };
    H2DE_LevelRect blue = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct PlayerSnap {
    bool bottom = true;
    bool top = true;
};

struct Checkpoint {
    H2DE_LevelPos pos = { 0.0f, 0.0f };
    int speed = 1;
    float velocityY = 0.0f;
    float rotation = 0.0f;

    PlayerGamemode gamemode = PLAYER_GAMEMODE_CUBE;
    PlayerSize size = PLAYER_SIZE_NORMAL;
    PlayerGravity gravity = PLAYER_GRAVITY_NORMAL;

    std::optional<int> gamemodeEntryPosY = std::nullopt;
    std::optional<float> camPosY = std::nullopt;
};

struct SurfaceDataBuffer {
    H2DE_SurfaceData surD = H2DE_SurfaceData();
    std::optional<H2DE_TextureData> texD = std::nullopt;
    std::optional<H2DE_SpriteData> sprD = std::nullopt;
};

struct BarObjectBuffer {
    H2DE_BarObjectData bod;
    std::unordered_map<std::string, SurfaceDataBuffer> fsdb = {};
    std::unordered_map<std::string, SurfaceDataBuffer> bsdb = {};
};

struct BasicObjectBuffer {
    H2DE_BasicObjectData bod;
    std::unordered_map<std::string, SurfaceDataBuffer> sdb = {};
};

struct ButtonObjectBuffer {
    H2DE_ButtonObjectData bod = H2DE_ButtonObjectData();
    std::unordered_map<std::string, SurfaceDataBuffer> sdb = {};
};

struct TextObjectBuffer {
    H2DE_TextObjectData tod = H2DE_TextObjectData();
};

struct ObjectBuffer {
    ObjectType type;
    H2DE_ObjectData od = H2DE_ObjectData();
    std::optional<BarObjectBuffer> barBuffer = std::nullopt;
    std::optional<BasicObjectBuffer> basicBuffer = std::nullopt;
    std::optional<ButtonObjectBuffer> buttonBuffer = std::nullopt;
    std::optional<TextObjectBuffer> textBuffer = std::nullopt;
};

struct IconData {
    H2DE_AbsRect col1SrcRect;
    H2DE_AbsRect col2SrcRect;
    H2DE_AbsRect glowSrcRect;
    H2DE_LevelRect col1DestRect;
    H2DE_LevelRect col2DestRect;
    H2DE_LevelRect glowDestRect;
};

struct BlockData {
    struct SurfaceData {
        std::string sheet = "";
        H2DE_AbsRect src = { 0, 0, 0, 0 };
        H2DE_LevelRect dest = { 0.0f, 0.0f, 1.0f, 1.0f };
        H2DE_ColorRGB defaultColor = { 255, 255, 255, 255 };
        float rotation = 0.0f;

        inline bool isNotOnScreenAnymore(float blockPosX, float camPosX) const {
            return (blockPosX + dest.x + dest.w < camPosX);
        }
    };

    struct HitboxData {
        H2DE_LevelRect dest = { 0.0f, 0.0f, 1.0f, 1.0f };
        H2DE_ColorRGB color = { 0, 0, 0, 0 };
    };

    BlockType type = BLOCK_TYPE_DECORATION;
    std::vector<std::vector<SurfaceData>> surfaces = {};
    std::optional<HitboxData> hitbox = std::nullopt;
    float rotation = 0.0f;
    H2DE_Flip flip = H2DE_FLIP_NONE;
    int index = 0;
};

struct TriggerData {
    TriggerType type = TRIGGER_TYPE_BACKGROUND_COLOR;
    H2DE_ColorRGB color = { 255, 255, 255, 255 };
    int duration = 0.5f;
    bool touchTrigger = false;
};

struct ItemData {
    std::string id = "";
    H2DE_LevelPos pos = { 0.0f, 0.0f };

    std::optional<BlockData> blockData = std::nullopt;
    std::optional<TriggerData> triggerData = std::nullopt;

    float minX = 0.0f;
    float maxX = 0.0f;

    void initMaximums() {
        if (blockData.has_value()) {
            for (const std::vector<BlockData::SurfaceData>& surfaceContainer : blockData.value().surfaces) {
                for (const BlockData::SurfaceData& surface : surfaceContainer) {

                    if (surface.dest.x < minX) {
                        minX = surface.dest.x;
                    }

                    if (surface.dest.x + surface.dest.w > maxX) {
                        maxX = surface.dest.x + surface.dest.w;
                    }
                }
            }
        }

        minX += pos.x;
        maxX += pos.x;
    }
};

#endif
