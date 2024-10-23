#ifndef UTILS_H
#define UTILS_H

struct LevelPos {
    float x;
    float y;
};

struct LevelSize {
    float w;
    float h;
};

struct LevelOffset {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Color {
    Uint8 r, g, b, a;

    operator H2DE_RGB() const {
        return { r, g, b, a };
    }
};

struct Hack {
    bool active;
    SDL_KeyCode keycode;
};

enum Face {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
};

struct Rect {
    float x, y, w, h;

    float getMinX() { return x; };
    float getMaxX() { return x + w; };
    float getMinY() { return y; };
    float getMaxY() { return y + h; };

    static bool intersect(Rect* rect1, Rect* rect2) {
        return (
            rect1->getMinX() < rect2->getMaxX() &&
            rect1->getMaxX() > rect2->getMinX() &&
            rect1->getMinY() < rect2->getMaxY() &&
            rect1->getMaxY() > rect2->getMinY()
        );
    };

    static Face getCollidedFace(Rect* rect1, Rect* rect2) {
        float dr = rect1->getMaxX() - rect2->getMinX();
        float dl = rect2->getMaxX() - rect1->getMinX();
        float dt = rect1->getMaxY() - rect2->getMinY();
        float db = rect2->getMaxY() - rect1->getMinY();

        float dmin = std::min({ dr, dl, db, dt });

        if (dr == dmin) return RIGHT;
        else if (dt == dmin) return TOP;
        else if (db == dmin) return BOTTOM;
        else {
            if (db > dt) return TOP;
            else return BOTTOM;
            return LEFT;
        }
    }
};

enum MainGameState {
    LOADING_SCREEN,
    MAIN_MENU,
    LEVEL_MENU,
    ICONS_MENU,
    LEVEL_STARTING_DELAY,
    LEVEL_PLAYING,
    LEVEL_DEAD,
    LEVEL_PAUSE,
    LEVEL_END,
};

enum SubGameState {
    DEFAULT,
    TRANSITION_IN,
    TRANSITION_OUT,
    MODAL_EXIT,
};

struct GameState {
    MainGameState main;
    SubGameState sub;

    bool operator==(const GameState& other) const {
        return (main == other.main && sub == other.sub);
    }
};

struct KeyEvent {
    SDL_Keycode keycode;
    std::vector<GameState> states;
    std::function<void()> call;
};

struct UpdateInstruction {
    std::vector<GameState> states;
    std::function<void()> call;
};

struct RenderInstruction {
    std::vector<GameState> states;
    std::function<void()> call;
};

enum Layer {
    BG, B5, B4, B3, B2, B1, P, T1, T2, T3, T4, G, H, UI
};

class Zindex {
private:
    Layer layer;
    int order;
    int range = 100;
    
public:
    Zindex(Layer l, int o) : layer(l), order(o) {};
    static Layer getLayer(std::string layer) {
        static std::unordered_map<std::string, Layer> layers = {
            { "BG", BG }, { "B5", B5 }, { "B4", B4 }, { "B3", B3 }, { "B2", B2 }, { "B1", B1 }, { "P", P },
            { "T1", T1 }, { "T2", T2 }, { "T3", T3 }, { "T4", T4 }, { "G", G }, { "H", H }, { "UI", UI }
        };
        return layers[layer];
    }
    int getIndex() const { return layer * range * 2 + layer + std::clamp(order, -range, range) + range; };
    int operator +(const Zindex& other) const { return this->getIndex() + other.getIndex(); }
    int operator -(const Zindex& other) const { return this->getIndex() - other.getIndex(); }
    bool operator<(const Zindex& other) const { return this->getIndex() < other.getIndex(); }
    bool operator>(const Zindex& other) const { return this->getIndex() > other.getIndex(); }
};

enum BlockSpecialDataType {
    SD_PORTAL = 0,
    SD_ORB = 1,
    SD_PAD = 2,
    SD_COIN = 3,
};

enum BlockSpecialDataDesc {
    SD_CUBE = 0,
    SD_SHIP = 1,
    SD_RIGHT_SIDE_UP = 2,
    SD_UPSIDE_DOWN = 3,
    SD_YELLOW = 4,
    SD_PINK = 5,
    SD_BLUE = 6,
    SD_SECRET = 7,
};

struct BlockSpecialData {
    BlockSpecialDataType type;
    BlockSpecialDataDesc desc;
};

enum LevelColor {
    COL_BLACK = -2,
    COL_WHITE = -1,
    COL_OBJ = 0,
    COL_1 = 1,
    COL_2 = 2,
    COL_3 = 3,
    COL_4 = 4,
    COL_P_1 = 5,
    COL_P_2 = 6,
};

struct BlockTextureData {
    std::string texture;
    int sprites;
    LevelSize size;
    LevelOffset offset;
    LevelPos origin;
    LevelColor color;
    Zindex zIndex;
};

struct BlockGlowData {
    int id;
    LevelSize size;
    LevelOffset offset;
    LevelPos origin;
};

struct BlockHitboxData {
    LevelSize size;
    LevelOffset offset;
    LevelPos origin;
};

enum BlockType {
    SOLID,
    OBSTACLE,
    SPECIAL,
    DECORATION,
};

struct BlockData {
    BlockType type;
    std::optional<BlockTextureData*> baseTexture;
    std::optional<BlockTextureData*> detailTexture;
    std::optional<BlockGlowData*> glow;
    std::optional<BlockHitboxData*> hitboxData;
    std::optional<BlockSpecialData*> specialData;
};

struct BufferedBlock {
    std::string id;
    BlockData* data;
    LevelPos pos = { 0.0f, 0.0f };
    int rotation = 0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    std::optional<LevelColor> baseColor = std::nullopt;
    std::optional<LevelColor> detailColor = std::nullopt;
    std::optional<Zindex> zIndex = std::nullopt;
};

enum TriggerType {
    T_STARTPOS,
    T_BACKGROUND,
    T_GROUND,
    T_LINE,
    T_BE_FADE,
    T_BE_FROM_TOP,
    T_BE_FROM_BOTTOM,
    T_BE_FROM_LEFT,
    T_BE_FROM_RIGHT,
    T_BE_SCALE,
};

struct TriggerData {
    TriggerType type;
};

struct BufferedTrigger {
    std::string id;
    TriggerData* data;
    LevelPos pos = { 0, 0 };
    std::optional<Color> color = std::nullopt;
    std::optional<unsigned int> ms = std::nullopt;
    bool touchTrigger = false;
};

enum Gravity {
    RIGHT_SIDE_UP = 1,
    UPSIDE_DOWN = -1,
};

enum Size {
    MINI,
    BIG,
};

enum Gamemode {
    CUBE,
    SHIP,
};

struct Checkpoint {
    LevelPos camPos = { 0, 0 };
    LevelPos playerPos = { 0, 0 };
    Velocity velocity = { 0, 0 };
    int speed = 1;
    Gravity gravity = RIGHT_SIDE_UP;
    Size size = BIG;
    Gamemode gamemode = CUBE;
    int rotation = 0;
    float botGroundPosY = -1.0;
};

enum LevelDifficulty {
    NA_0,
    AUTO_1,
    EASY_2,
    NORMAL_3,
    HEAD_4,
    HEAD_5,
    HEADER_6,
    HEADER_7,
    INSANE_8,
    INSANE_9,
    EASY_DEMON_10,
    MEDIUM_DEMON_10,
    HARD_DEMON_10,
    INSANE_DEMON_10,
    EXTREME_DEMON_10,
};

struct LevelData {
    int id;
    std::string name;
    LevelDifficulty difficulty;
    Checkpoint startpos;
    Color menuColor;
    Color backgroundColor;
    Color groundColor;
    Color lineColor;
    std::string backgroundTexture;
    std::string groundTexture;
    std::string lineTexture;
    std::string song;
    std::vector<BufferedBlock*> blocks = {};
    std::vector<BufferedTrigger*> triggers = {};
    std::vector<Color> colors = {};
};

struct Icons {
    std::unordered_map<Gamemode, int> ids;

    std::vector<int> colorIDs;
    bool glow;
};

struct Hitbox {
    LevelPos pos;
    Gamemode gamemode;
    Size size;
};

struct MenuIcon {
    int id;

    LevelPos pos;
    Velocity velocity;
    Gamemode gamemode;
    Size size;
    int speed;
    int rotation;

    int col1;
    int col2;
    bool glow;

    bool onSolid;
    bool holding;
};

enum LevelMode {
    NORMAL_MODE,
    PRACTICE_MODE,
};

enum BlockEffect {
    FADE,
    FROM_TOP,
    FROM_BOTTOM,
    FROM_LEFT,
    FROM_RIGHT,
    SCALE,
};

enum Pad {
    YELLOW_PAD,
    PINK_PAD,
    BLUE_PAD,
};

enum Orb {
    YELLOW_ORB,
    PINK_ORB,
    BLUE_ORB,
};

#endif
