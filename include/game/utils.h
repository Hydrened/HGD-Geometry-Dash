#ifndef UTILS_H
#define UTILS_H

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
    PLAYER_GRAVITY_NORMAL = 1,
    PLAYER_GRAVITY_UPSIDE_DOWN = -1,
};

struct Checkpoint {
    H2DE_LevelPos pos;
    H2DE_LevelVelocity velocity;
    float rotation;

    PlayerGamemode gamemode;
    PlayerSize size;
    PlayerGravity gravity;

    int gamemodeEntryPosY;
    H2DE_LevelPos camPos;
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

struct ItemBuffer {
    std::string id;
    H2DE_LevelPos pos;
};

struct BlockBuffer {
    float rotation;
    H2DE_Flip flip;
};

struct TriggerBuffer {
    int duration;
};

struct BlockData {
    struct SurfaceData {
        std::string sheet;
        H2DE_AbsRect src;
        H2DE_LevelRect dest = { 0.0f, 0.0f, 1.0f, 1.0f };
        H2DE_ColorRGB defaultColor = { 255, 255, 255, 255 };
    };

    std::vector<SurfaceData> surfaces = {};
    int index;
};

#endif
