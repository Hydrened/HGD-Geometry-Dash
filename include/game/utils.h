#ifndef UTILS_H
#define UTILS_H

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

enum Layer {
    BG, B5, B4, B3, B2, B1, P, T1, T2, T3, T4, G, H, UI
};

enum BlockType {
    SOLID,
    OBSTACLE,
    SPECIAL,
    DECORATION,
};

enum TriggerType {
    STARTPOS,
    BACKGROUND,
    GROUND,
    LINE,
};

enum TriggerState {
    WAITING,
    ACTIVATING,
    PASSED,
};

enum Face {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
};

enum LevelMode {
    NORMAL_MODE,
    PRACTICE_MODE,
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

enum ItemSpecialDataType {
    SD_PORTAL = 0,
    SD_ORB = 1,
    SD_PAD = 2,
    SD_COIN = 3,
};

enum ItemSpecialDataDesc {
    SD_CUBE = 0,
    SD_SHIP = 1,
    SD_RIGHT_SIDE_UP = 2,
    SD_UPSIDE_DOWN = 3,
    SD_YELLOW = 4,
    SD_PINK = 5,
    SD_BLUE = 6,
    SD_SECRET = 7,
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

    explicit operator H2DE_RGB() const {
        return { r, g, b, a };
    }
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
    int getIndex() { return layer * range * 2 + layer + order; };
};

struct ItemSpecialData {
    ItemSpecialDataType type;
    ItemSpecialDataDesc desc;
};

struct BufferedBlock {
    BlockType type;
    std::string texture = "";
    LevelPos pos = { 0, 0 };
    LevelSize textureSize = { 0, 0 };
    LevelSize hitboxSize = { 0, 0 };
    LevelOffset textureOffset = { 0, 0 };
    LevelOffset hitboxOffset = { 0, 0 };
    LevelPos rotationOrigin = { 0, 0 };
    int rotation = 0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int colorID = 0;
    ItemSpecialData specialData;
    int sprites = 0;
    Zindex* zIndex;
};

struct BufferedTrigger {
    TriggerType type;
    LevelPos pos = { 0, 0 };
    Color color = { 0, 0, 0, 0 };
    int ms = 0;
    bool touchTrigger = false;
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
    std::vector<Color> colors = {{ 255, 255, 255, 255 }};
};

struct Hack {
    bool active;
    SDL_KeyCode keycode;
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

#endif
