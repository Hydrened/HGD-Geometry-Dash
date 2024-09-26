#ifndef DATA_H
#define DATA_H

struct GameData {
    struct Positions {
        LevelPos camera = { -6.5f, -3.0f };
        LevelPos backgroundPos = { -6.5f, -4.0f };
        LevelPos botGroundPos = { -6.5f, -4.0f };
        LevelPos topGroundPos = { -6.5f, 80.0f };
        LevelPos botLinePos = { -6.5f, -4.0f };
        LevelPos topLinePos = { -6.5f, 80.0f };
        float cameraMinY = -3.0f;
        float cameraMaxY = 80.0f;
        LevelPos gameTitle = { 13.05f, 8.89f };
        LevelPos levelMenuButton = { 41.33f, 31.67f };
        LevelPos iconsMenuButton = { 25.16f, 37.22f };
        LevelPos onlineMenuButton = { 63.83f, 37.22f };
        LevelPos progressBar = { 0.0f, 0.0f };
    };

    struct Sizes {
        LevelSize background = { 19.0f, 19.0f };
        LevelSize ground = { 4.0f, 4.0f };
        LevelSize line = { 15.0f, 0.05 }; // replace => 15.0f(more accurate value)
        LevelSize checkpoint = { 1.0f, 1.0f };
        float levelHeight = 80.0f;
        float cameraPaddingTop = 3.0f; // replace => 3.0f(more accurate value)
        float cameraPaddingBot = 3.0f; // replace => 3.0f(more accurate value)
        float levelEndPadding = 8.0f; // replace => 8.0f(more accurate value)
        std::unordered_map<Gamemode, float> gamemodeHeights;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> iconSizes;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> redHitboxSizes;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> blueHitboxSizes;
        LevelSize gameTitle = { 74.61f, 14.86f };
        LevelSize levelMenuButton = { 18.28f, 32.5f };
        LevelSize iconsMenuButton = { 11.79f, 20.97f };
        LevelSize onlineMenuButton = { 11.79f, 20.97f };
        LevelSize progressBar = { 100.0f, 2.0f };

        Sizes() {
            gamemodeHeights[CUBE] = -1.0f;
            gamemodeHeights[SHIP] = 10.0f;
            iconSizes[CUBE][NORMAL] = { 1.0f, 1.0f };
            iconSizes[CUBE][MINI] = { 1.0f, 1.0f };
            iconSizes[SHIP][NORMAL] = { 1.0f, 1.0f };
            iconSizes[SHIP][MINI] = { 1.0f, 1.0f };
            redHitboxSizes[CUBE][NORMAL] = { 1.0f, 1.0f };
            redHitboxSizes[CUBE][MINI] = { 1.0f, 1.0f };
            redHitboxSizes[SHIP][NORMAL] = { 1.0f, 1.0f };
            redHitboxSizes[SHIP][MINI] = { 1.0f, 1.0f };
            blueHitboxSizes[CUBE][NORMAL] = { 0.35f, 0.35f };
            blueHitboxSizes[CUBE][MINI] = { 0.35f, 0.35f };
            blueHitboxSizes[SHIP][NORMAL] = { 0.35f, 0.35f };
            blueHitboxSizes[SHIP][MINI] = { 0.35f, 0.35f };
        }
    };

    struct Offsets {
        LevelOffset botLine = { 2.0f, 3.95f };
        LevelOffset topLine = { 2.0f, 0.0f };
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> iconOffsets;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> redHitboxOffsets;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> blueHitboxOffsets;

        Offsets() {
            iconOffsets[CUBE][NORMAL] = { 0.0f, 0.0f };
            iconOffsets[CUBE][MINI] = { 0.0f, 0.0f };
            iconOffsets[SHIP][NORMAL] = { 0.0f, 0.0f };
            iconOffsets[SHIP][MINI] = { 0.0f, 0.0f };
            redHitboxOffsets[CUBE][NORMAL] = { 0.0f, 0.0f };
            redHitboxOffsets[CUBE][MINI] = { 0.0f, 0.0f };
            redHitboxOffsets[SHIP][NORMAL] = { 0.0f, 0.0f };
            redHitboxOffsets[SHIP][MINI] = { 0.0f, 0.0f };
            blueHitboxOffsets[CUBE][NORMAL] = { 0.325f, -0.325f };
            blueHitboxOffsets[CUBE][MINI] = { 0.325f, -0.325f };
            blueHitboxOffsets[SHIP][NORMAL] = { 0.325f, -0.325f };
            blueHitboxOffsets[SHIP][MINI] = { 0.325f, -0.325f };
        }
    };

    struct Physics {
        std::vector<float> speeds = { 0.0f, 0.1723f, 0.0f, 0.0f, 0.0f }; // replace => more accurate values 0.171
        float backgroundRatio = 0.9f;
        float cameraSpeedY = 0.05f;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> gravities;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> maxGravities;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> clicks;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> rotations;
        std::unordered_map<Gamemode, bool> canHitTop;
        std::unordered_map<Gamemode, bool> canHitBottom;
        std::unordered_map<Gamemode, bool> canMoveCamera;

        Physics() {
            gravities[CUBE][NORMAL] = -0.0244f;
            gravities[CUBE][MINI] = 0.0f;
            gravities[SHIP][NORMAL] = -0.013f;
            gravities[SHIP][MINI] = 0.0f;
            maxGravities[CUBE][NORMAL] = 0.4f;
            maxGravities[CUBE][MINI] = 0.0f;
            maxGravities[SHIP][NORMAL] = 0.22f;
            maxGravities[SHIP][MINI] = 0.0f;
            clicks[CUBE][NORMAL] = 0.3332f;
            clicks[CUBE][MINI] = 0.0f;
            clicks[SHIP][NORMAL] = 0.03f;
            clicks[SHIP][MINI] = 0.0f;
            rotations[CUBE][NORMAL] = 6.8f;
            rotations[CUBE][MINI] = 0.0f;
            rotations[SHIP][NORMAL] = -200.0f;
            rotations[SHIP][MINI] = 0.0f;
            canHitTop[CUBE] = false;
            canHitTop[SHIP] = true;
            canHitBottom[CUBE] = true;
            canHitBottom[SHIP] = true;
            canMoveCamera[CUBE] = true;
            canMoveCamera[SHIP] = false;
        }
    };

    struct Colors {
        std::unordered_map<BlockType, Color> hitboxes;

        Colors() {
            hitboxes[SOLID] = { 128, 128, 255, 255 };
            hitboxes[OBSTACLE] = { 255, 128, 128, 255 };
            hitboxes[SPECIAL] = { 128, 255, 128, 255 };
            hitboxes[DECORATION] = { 0, 0, 0, 0 };
        }
    };

    ~GameData() {
        delete positions;
        delete sizes;
        delete offsets;
        delete physics;
        delete colors;
    };

    Positions* positions = new Positions();
    Sizes* sizes = new Sizes();
    Offsets* offsets = new Offsets();
    Physics* physics = new Physics();
    Colors* colors = new Colors();
};


#endif
