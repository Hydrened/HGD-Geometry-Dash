#ifndef DATA_H
#define DATA_H

struct GameData {
    struct Positions {
        // level
        LevelPos camera = { -6.5f, -3.0f };
        LevelPos background = { -6.5f, -4.0f };
        LevelPos botGround = { -6.5f, -4.0f };
        LevelPos topGround = { -6.5f, 91.0f };
        LevelPos botLine = { -6.5f, -4.0f };
        LevelPos topLine = { -6.5f, 80.0f };
        float cameraMinY = -3.0f;
        float cameraMaxY = 80.0f;
        float backgroundMinY = -4.0f;
        float backgroundMaxY = 70.0f;
        LevelPos progressBar = { 0.0f, 0.0f };

        // main menu
        LevelPos gameTitle = { 13.05f, 8.89f };
        LevelPos levelMenuButton = { 41.33f, 31.67f };
        LevelPos shopMenuButton = { 25.16f, 37.22f };
        LevelPos onlineMenuButton = { 63.83f, 37.22f };
        LevelPos closeGameButton = { 0.57f, 1.2f };
    };

    struct Sizes {
        LevelSize background = { 19.0f, 19.0f };
        LevelSize ground = { 4.0f, 4.0f };
        LevelSize line = { 15.0f, 0.05 };
        LevelSize checkpoint = { 0.5f, 1.0f };
        float levelHeight = 80.0f;
        float cameraPaddingTop = 3.0f; 
        float cameraPaddingBot = 3.2f;
        float levelEndPadding = 8.0f;
        std::unordered_map<Gamemode, int> gamemodeHeights;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> mainIcon;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> redHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> blueHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> secondIcon;

        // main menu
        LevelSize gameTitle = { 74.61f, 14.86f };
        LevelSize levelMenuButton = { 18.28f, 32.5f };
        LevelSize shopMenuButton = { 11.79f, 20.97f };
        LevelSize onlineMenuButton = { 11.79f, 20.97f };
        LevelSize progressBar = { 100.0f, 2.0f };
        LevelSize closeGameButton = { 5.52f, 10.0f };

        Sizes() {
            gamemodeHeights[CUBE] = -1;
            gamemodeHeights[SHIP] = 10;

            mainIcon[CUBE][BIG] = { 1.0f, 1.0f };
            mainIcon[CUBE][MINI] = { 1.0f, 1.0f };
            mainIcon[SHIP][BIG] = { 1.8f, 1.0f };
            mainIcon[SHIP][MINI] = { 1.0f, 1.0f };

            secondIcon[CUBE][MINI] = { 0.0f, 0.0f };
            secondIcon[CUBE][BIG] = { 1.0f, 1.0f };
            secondIcon[SHIP][MINI] = { 0.0f, 0.0f };
            secondIcon[SHIP][BIG] = { 0.6f, 0.6f };

            redHitbox[CUBE][BIG] = { 1.0f, 1.0f };
            redHitbox[CUBE][MINI] = { 1.0f, 1.0f };
            redHitbox[SHIP][BIG] = { 1.0f, 1.0f };
            redHitbox[SHIP][MINI] = { 1.0f, 1.0f };

            blueHitbox[CUBE][BIG] = { 0.35f, 0.35f };
            blueHitbox[CUBE][MINI] = { 0.35f, 0.35f };
            blueHitbox[SHIP][BIG] = { 0.35f, 0.35f };
            blueHitbox[SHIP][MINI] = { 0.35f, 0.35f };
        }
    };

    struct Offsets {
        LevelOffset botLine = { 2.0f, 3.95f };
        LevelOffset topLine = { 2.0f, 0.0f };
        LevelOffset checkpoint = { 0.25f, 0.0f };
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> mainIcon;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> redHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> blueHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelOffset>> secondIcon;

        Offsets() {
            mainIcon[CUBE][BIG] = { 0.0f, 0.0f };
            mainIcon[CUBE][MINI] = { 0.0f, 0.0f };
            mainIcon[SHIP][BIG] = { -0.38, -0.19 };
            mainIcon[SHIP][MINI] = { 0.0f, 0.0f };

            secondIcon[CUBE][MINI] = { 0.0f, 0.0f };
            secondIcon[CUBE][BIG] = { 0.0f, 0.0f };
            secondIcon[SHIP][MINI] = { 0.0f, 0.0f };
            secondIcon[SHIP][BIG] = { 0.2f, 0.384f };

            redHitbox[CUBE][BIG] = { 0.0f, 0.0f };
            redHitbox[CUBE][MINI] = { 0.0f, 0.0f };
            redHitbox[SHIP][BIG] = { 0.0f, 0.0f };
            redHitbox[SHIP][MINI] = { 0.0f, 0.0f };

            blueHitbox[CUBE][BIG] = { 0.325f, 0.325f };
            blueHitbox[CUBE][MINI] = { 0.325f, 0.325f };
            blueHitbox[SHIP][BIG] = { 0.325f, 0.325f };
            blueHitbox[SHIP][MINI] = { 0.325f, 0.325f };
        }
    };

    struct Physics {
        std::vector<float> speeds = { 0.0f, 0.1723f, 0.0f, 0.0f, 0.0f };
        float backgroundRatio = 0.9f;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> gravities;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> maxGravities;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> clicks;
        std::unordered_map<Gamemode, std::unordered_map<Size, float>> rotations;
        std::unordered_map<Pad, std::unordered_map<Gamemode, std::unordered_map<Size, float>>> pads;
        std::unordered_map<Orb, std::unordered_map<Gamemode, std::unordered_map<Size, float>>> orbs;
        std::unordered_map<Gamemode, bool> canHitTop;
        std::unordered_map<Gamemode, bool> canHitBottom;
        std::unordered_map<Gamemode, bool> canMoveCamera;

        Physics() {
            gravities[CUBE][BIG] = -0.0244;
            gravities[CUBE][MINI] = 0.0f;
            gravities[SHIP][BIG] = -0.011f;
            gravities[SHIP][MINI] = 0.0f;

            maxGravities[CUBE][BIG] = 0.4f;
            maxGravities[CUBE][MINI] = 0.0f;
            maxGravities[SHIP][BIG] = 0.22f;
            maxGravities[SHIP][MINI] = 0.0f;

            clicks[CUBE][BIG] = 0.3332;
            clicks[CUBE][MINI] = 0.0f;
            clicks[SHIP][BIG] = 0.03;
            clicks[SHIP][MINI] = 0.0f;

            rotations[CUBE][BIG] = 7.0f;
            rotations[CUBE][MINI] = 7.0f;
            rotations[SHIP][BIG] = -200.0f;
            rotations[SHIP][MINI] = -200.0f;

            pads[YELLOW_PAD][CUBE][BIG] = 1.3f;
            pads[YELLOW_PAD][CUBE][MINI] = 0.0f;
            pads[YELLOW_PAD][SHIP][BIG] = 0.0f;
            pads[YELLOW_PAD][SHIP][MINI] = 0.0f;

            pads[PINK_PAD][CUBE][BIG] = 0.31f;
            pads[PINK_PAD][CUBE][MINI] = 0.0f;
            pads[PINK_PAD][SHIP][BIG] = 0.0f;
            pads[PINK_PAD][SHIP][MINI] = 0.0f;
            
            pads[BLUE_PAD][CUBE][BIG] = 0.18f;
            pads[BLUE_PAD][CUBE][MINI] = 0.0f;
            pads[BLUE_PAD][SHIP][BIG] = 0.0f;
            pads[BLUE_PAD][SHIP][MINI] = 0.0f;

            orbs[YELLOW_ORB][CUBE][BIG] = 0.3332f;
            orbs[YELLOW_ORB][CUBE][MINI] = 0.0f;
            orbs[YELLOW_ORB][SHIP][BIG] = 0.0f;
            orbs[YELLOW_ORB][SHIP][MINI] = 0.0f;

            orbs[PINK_ORB][CUBE][BIG] = 0.2f;
            orbs[PINK_ORB][CUBE][MINI] = 0.0f;
            orbs[PINK_ORB][SHIP][BIG] = 0.0f;
            orbs[PINK_ORB][SHIP][MINI] = 0.0f;
            
            orbs[BLUE_ORB][CUBE][BIG] = 0.18f;
            orbs[BLUE_ORB][CUBE][MINI] = 0.0f;
            orbs[BLUE_ORB][SHIP][BIG] = 0.0f;
            orbs[BLUE_ORB][SHIP][MINI] = 0.0f;

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
        std::vector<Color> icons = {
            { 125, 255, 0, 255 },
            { 0, 255, 0, 255 },
            { 0, 255, 125, 255 },
            { 0, 255, 255, 255 },
            { 0, 125, 255, 255 },
            { 0, 0, 255, 255 },
            { 125, 0, 255, 255 },
            { 255, 0, 255, 255 },
            { 255, 0, 125, 255 },
            { 255, 0, 0, 255 },
            { 255, 125, 0, 255 },
            { 255, 225, 0, 255 },
        };

        Colors() {
            hitboxes[SOLID] = { 128, 128, 255, 255 };
            hitboxes[OBSTACLE] = { 255, 128, 128, 255 };
            hitboxes[SPECIAL] = { 128, 255, 128, 255 };
            hitboxes[DECORATION] = { 0, 0, 0, 0 };
        }
    };

    struct Other {
        std::unordered_map<Gamemode, bool> iconSecondTexture;
        std::vector<std::string> gamemodeStringified = { "cube", "ship" };

        Other() {
            iconSecondTexture[CUBE] = false;
            iconSecondTexture[SHIP] = true;
        }
    };

    ~GameData() {
        delete positions;
        delete sizes;
        delete offsets;
        delete physics;
        delete colors;
        delete other;
    };

    Positions* positions = new Positions();
    Sizes* sizes = new Sizes();
    Offsets* offsets = new Offsets();
    Physics* physics = new Physics();
    Colors* colors = new Colors();
    Other* other = new Other();
};

#endif
