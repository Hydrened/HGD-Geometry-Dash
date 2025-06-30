#ifndef UTILS_H
#define UTILS_H

#include <H2DE/utils/H2DE_utils.h>
#include <SDL2/SDL_events.h>
#include <optional>

class H2DE_BasicObject;
class H2DE_Texture;

using Icon_ID = uint8_t;
using Level_ID = uint32_t;
using Color_ID = uint8_t;
using Speed = uint8_t;
using H2DE_Velocity = H2DE_Vector2D<float>;

enum GameState {
    GAME_STATE_LOADING_ASSETS,
    GAME_STATE_MENU,
    GAME_STATE_LEVEL,
};

enum MenuID {
    MENU_ID_MAIN_MENU,
    MENU_ID_LEVEL_MENU,
    MENU_ID_ICON_MENU,
};

enum ModalID {
    MODAL_ID_QUIT_GAME,
};

enum TransitionState {
    TRANSITION_STATE_IN,
    TRANSITION_STATE_OUT,
};

enum Layer {
    LAYER_BG    = 0b0001,
    LAYER_B5    = 0b0010,
    LAYER_B4    = 0b0011,
    LAYER_B3    = 0b0100,
    LAYER_B2    = 0b0101,
    LAYER_B1    = 0b0110,
    LAYER_P     = 0b0111,
    LAYER_T1    = 0b1000,
    LAYER_T2    = 0b1001,
    LAYER_T3    = 0b1010,
    LAYER_T4    = 0b1011,
    LAYER_G     = 0b1100,
    LAYER_H     = 0b1101,
    LAYER_UI    = 0b1110,
};

enum PlayerGamemode {
    PLAYER_GAMEMODE_CUBE    = 0,
    PLAYER_GAMEMODE_SHIP    = 1,
    PLAYER_GAMEMODE_BALL    = 2,
    PLAYER_GAMEMODE_UFO     = 3,
    PLAYER_GAMEMODE_WAVE    = 4,
};

enum PlayerSize {
    PLAYER_SIZE_NORMAL,
    PLAYER_SIZE_MINI,
};

enum PlayerGravity {
    PLAYER_GRAVITY_NORMAL           = 1,
    PLAYER_GRAVITY_UPSIDE_DOWN      = -1,
};

enum BlockType {
    BLOCK_TYPE_SOLID        = 0,
    BLOCK_TYPE_OBSTACLE     = 1,
    BLOCK_TYPE_SPECIAL      = 2,
    BLOCK_TYPE_DECORATION   = 3,
};

enum TriggerType {
    TRIGGER_TYPE_STARTPOS           = 1,
    TRIGGER_TYPE_BACKGROUND_COLOR   = 2,
    TRIGGER_TYPE_GROUND_COLOR       = 3,
    TRIGGER_TYPE_LINE_COLOR         = 4,
    TRIGGER_TYPE_BE_FADE            = 108,
    TRIGGER_TYPE_BE_FROM_TOP        = 109,
    TRIGGER_TYPE_BE_FROM_BOTTOM     = 110,
    TRIGGER_TYPE_BE_FROM_LEFT       = 111,
    TRIGGER_TYPE_BE_FROM_RIGHT      = 112,
    TRIGGER_TYPE_BE_FROM_SCALE      = 113,
};

struct PlayerHitbox {
    H2DE_LevelRect red = H2DE_LevelRect();
    H2DE_LevelRect blue = H2DE_LevelRect();
};

struct PlayerIcons {
    Icon_ID cubeID = 1;
    Icon_ID shipID = 1;
    Color_ID col1 = 1;
    Color_ID col2 = 1;
    bool glow = false;
};

struct PlayerSnap {
    bool bot = false;
    bool top = false;
};

struct Checkpoint {
    H2DE_Translate translate = H2DE_Translate();
    Speed speed = 1;
    float velocityY = 0.0f;
    float rotation = 0.0f;

    PlayerGamemode gamemode = PLAYER_GAMEMODE_CUBE;
    PlayerSize size = PLAYER_SIZE_NORMAL;
    PlayerGravity gravity = PLAYER_GRAVITY_NORMAL;

    std::optional<int> gamemodeEntryTranslateY = std::nullopt;
    std::optional<float> camTranslateY = std::nullopt;
};

struct BlockData {
    float rotation = 0.0f;
    H2DE_Scale flip = { 1.0f, 1.0f };
};

struct TriggerData {
    std::optional<H2DE_ColorRGB> color = std::nullopt;
    std::optional<int> duration = std::nullopt;
    bool touchTrigger = false;
};

struct ItemData {
    std::string id = "";
    H2DE_Translate translate = H2DE_Translate();
};

void openExternLink(const std::string& link);

void callShortcut(SDL_Keycode keycode);

void logLoadingTime(const std::function<void()>& loading, const std::string& name);

#endif
