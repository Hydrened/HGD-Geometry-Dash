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
    TRANSITION_OUT,
    TRANSITION_IN,
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

struct GameState {
    MainGameState main;
    SubGameState sub;
};

struct KeyEvent {
    SDL_Keycode keycode;
    GameState state;
    std::function<void()> call;
};

struct UpdateInstruction {
    GameState state;
    std::function<void()> call;
};

struct RenderInstruction {
    GameState state;
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

    explicit operator H2DE_Color() const {
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
        float overlapLeft = rect1->x + rect1->w - rect2->x;
        float overlapTop = rect1->y + rect1->h - rect2->y;
        float overlapBottom = rect2->y + rect2->h - rect1->y;
        float minOverlap = std::min({ overlapLeft, overlapTop, overlapBottom });
        
        if (minOverlap == overlapTop) return TOP;
        else if (minOverlap == overlapBottom) return BOTTOM;
        else return RIGHT;
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

struct BufferedBlock {
    BlockType type;
    std::string texture = "";
    LevelPos pos = { 0, 0 };
    LevelSize textureSize = { 0, 0 };
    LevelSize hitboxSize = { 0, 0 };
    LevelOffset textureOffset = { 0, 0 };
    LevelOffset hitboxOffset = { 0, 0 };
    int rotation = 0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int colorID = 0;
    std::string specialData = "";
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

#endif
