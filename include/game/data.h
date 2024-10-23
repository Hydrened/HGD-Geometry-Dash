#ifndef DATA_H
#define DATA_H

#define BLOCKS_ON_WIDTH 19

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
        LevelPos practiceButtons = { 7.3f, 0.5f };

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
        LevelSize camera = { BLOCKS_ON_WIDTH + 0.3f, 10.5f }; // replace 10.5 => more accurate value
        float cameraPaddingBot = 3.2f;
        float cameraXremain = 0.3f;
        float levelHeight = 80.0f;
        float cameraPaddingTop = 3.0f; 
        float blockEffectPadding = 2.5f;
        float levelMinLength = 8.0f; // replace
        std::unordered_map<Gamemode, int> gamemodeHeights;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> mainIcon;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> redHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> blueHitbox;
        std::unordered_map<Gamemode, std::unordered_map<Size, LevelSize>> secondIcon;

        LevelSize practiceButtons = { 4.25f, 1.6f };

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
        float blockEffectMove = 3.0f;
        float levelEndWallX = -0.6f;
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
            gravities[SHIP][BIG] = -0.009;
            gravities[SHIP][MINI] = 0.0f;

            maxGravities[CUBE][BIG] = 0.4f;
            maxGravities[CUBE][MINI] = 0.0f;
            maxGravities[SHIP][BIG] = 0.22f;
            maxGravities[SHIP][MINI] = 0.0f;

            clicks[CUBE][BIG] = 0.3332;
            clicks[CUBE][MINI] = 0.0f;
            clicks[SHIP][BIG] = 0.023;
            clicks[SHIP][MINI] = 0.0f;

            rotations[CUBE][BIG] = 7.0f;
            rotations[CUBE][MINI] = 7.0f;
            rotations[SHIP][BIG] = -44.5;
            rotations[SHIP][MINI] = -44.5;

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
        Color menuBackground = { 0, 102, 255, 255 };
        Color menuGround = { 0, 0, 216, 255 };
        std::unordered_map<BlockType, Color> hitboxes;
        float glowOpacity = 0.6f;
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
            hitboxes[SOLID] = { 0, 0, 255, 255 };
            hitboxes[OBSTACLE] = { 255, 0, 0, 255 };
            hitboxes[SPECIAL] = { 0, 255, 0, 255 };
            hitboxes[DECORATION] = { 0, 0, 0, 0 };
        }
    };

    struct Other {
        std::unordered_map<Gamemode, bool> iconSecondTexture;
        std::vector<std::string> gamemodeStringified = { "cube", "ship" };
        std::vector<TriggerType> colorTriggers = { T_BACKGROUND, T_GROUND, T_LINE };
        std::vector<TriggerType> blockEffectTriggers = { T_BE_FADE, T_BE_FROM_TOP, T_BE_FROM_BOTTOM, T_BE_FROM_LEFT, T_BE_FROM_RIGHT, T_BE_SCALE };
        std::unordered_map<std::string, BlockData> blocks;
        std::unordered_map<std::string, TriggerData> triggers;

        Other() {
            iconSecondTexture[CUBE] = false;
            iconSecondTexture[SHIP] = true;

            blocks["0_1"] = {
                SPECIAL,
                new BlockTextureData{ "0_1.png", 4, { 1.34f, 1.34f }, { -0.17f, -0.17f }, { 0.67f, 0.67f }, COL_WHITE, { T1, 9 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 1.5f, 1.5f }, { -0.25f, -0.25f }, { 0.75f, 0.75f } },
                new BlockSpecialData{ SD_COIN, SD_SECRET },
            };

            blocks["1_1"] = {
                SOLID,
                new BlockTextureData{ "1_1.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 1, { 1.3f, 1.3f }, { -0.15f, -0.15f }, { 0.66f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_2"] = {
                SOLID,
                new BlockTextureData{ "1_2.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 1, { 1.3f, 1.3f }, { -0.15f, -0.15f }, { 0.66f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_3"] = {
                SOLID,
                new BlockTextureData{ "1_3.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 5, { 1.0f, 0.4f }, { 0.0f, 0.75f }, { 0.5f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_4"] = {
                SOLID,
                new BlockTextureData{ "1_4.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 3, { 1.15f, 1.15f }, { -0.15f, 0.0f }, { 0.66f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_5"] = {
                SOLID,
                new BlockTextureData{ "1_5.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 10, { 0.35f, 0.35f }, { -0.14f, 0.79f }, { 0.65f, 0.65f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_6"] = {
                DECORATION,
                new BlockTextureData{ "1_6.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { B2, -7 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["1_7"] = {
                DECORATION,
                new BlockTextureData{ "1_7.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { B2, -7 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["1_8"] = {
                SOLID,
                new BlockTextureData{ "1_8.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 2, { 1.3f, 1.15f }, { -0.15f, -0.01f }, { 0.66f, 0.65f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_9"] = {
                SOLID,
                new BlockTextureData{ "1_9.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 4, { 1.3f, 1.0f }, { -0.155f, 0.0f }, { 0.66f, 0.5f } },
                new BlockHitboxData{ { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_807"] = {
                SOLID,
                new BlockTextureData{ "1_807.png", 0, { 1.0f, 0.47f }, { 0.0f, 0.53f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 6, { 1.3f, 0.77f }, { -0.15f, 0.38f }, { 0.66f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 0.47f }, { 0.0f, 0.53f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_815"] = {
                SOLID,
                new BlockTextureData{ "1_815.png", 0, { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 5, { 1.0f, 0.4f }, { 0.0f, 0.75f }, { 0.5f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_816"] = {
                SOLID,
                new BlockTextureData{ "1_816.png", 0, { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 5, { 1.0f, 0.4f }, { 0.0f, 0.75f }, { 0.5f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_817"] = {
                SOLID,
                new BlockTextureData{ "1_817.png", 0, { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 5, { 1.0f, 0.4f }, { 0.0f, 0.75f }, { 0.5f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f } },
                std::nullopt,
            };
            blocks["1_818"] = {
                SOLID,
                new BlockTextureData{ "1_818.png", 0, { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 11, { 1.2f, 0.4f }, { -0.1f, 0.75f }, { 0.61f, 0.66f } },
                new BlockHitboxData{ { 1.0f, 0.53f }, { 0.0f, 0.47f }, { 0.5f, 0.5f } },
                std::nullopt,
            };

            blocks["4_1"] = {
                OBSTACLE,
                new BlockTextureData{ "4_1.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 7, { 1.2f, 1.2f }, { -0.09f, -0.15f }, { 0.6f, 0.56f } },
                new BlockHitboxData{ { 0.24f, 0.42f }, { 0.38f, 0.29f }, { 0.12f, 0.22f } },
                std::nullopt,
            };
            blocks["4_2"] = {
                OBSTACLE,
                new BlockTextureData{ "4_2.png", 0, { 1.0f, 0.45f }, { 0.0f, -0.02f }, { 0.5f, -0.08f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 8, { 1.15f, 0.7f }, { -0.075f, -0.15f }, { 0.58f, 0.04f } },
                new BlockHitboxData{ { 0.24f, 0.2f }, { 0.38f, 0.1f }, { 0.13, -0.21 } },
                std::nullopt,
            };
            blocks["4_3"] = {
                OBSTACLE,
                new BlockTextureData{ "4_3.png", 0, { 0.66f, 0.64f }, { 0.17f, -0.02f }, { 0.33f, 0.12f }, COL_OBJ, { T1, 2 } },
                std::nullopt,
                new BlockGlowData{ 9, { 0.86f, 0.86f }, { 0.06f, -0.15f }, { 0.43f, 0.21f } },
                new BlockHitboxData{ { 0.135f, 0.25f }, { 0.4325f, 0.18f }, { 0.07f, -0.07f } },
                std::nullopt,
            };
            blocks["4_31"] = {
                OBSTACLE,
                new BlockTextureData{ "4_31.png", 0, { 1.0f, 0.48f }, { 0.0f, -0.05f }, { 0.5f, -0.08f }, COL_WHITE, { T1, 2 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 0.34f, 0.25f }, { 0.33f, 0.0f }, { 0.17f, -0.26f } },
                std::nullopt,
            };
        
            blocks["6_1"] = {
                SPECIAL,
                new BlockTextureData{ "6_1.png", 0, { 0.84f, 0.15f }, { 0.08f, 0.0f }, { 0.42f, -0.35f }, COL_WHITE, { B1, 12 } },
                std::nullopt,
                std::nullopt, // glow
                new BlockHitboxData{ { 0.84f, 0.15f }, { 0.08f, 0.0f }, { 0.42f, -0.35f } },
                new BlockSpecialData{ SD_PAD, SD_YELLOW },
            };
            blocks["6_2"] = {
                SPECIAL,
                new BlockTextureData{ "6_2.png", 0, { 0.84f, 0.17f }, { 0.08f, -0.02f }, { 0.42f, -0.35f }, COL_WHITE, { B1, 12 } },
                std::nullopt,
                std::nullopt, // glow
                new BlockHitboxData{ { 0.84f, 0.17f }, { 0.08f, -0.02f }, { 0.42f, -0.35f } },
                new BlockSpecialData{ SD_PAD, SD_PINK },
            };
            blocks["6_4"] = {
                SPECIAL,
                new BlockTextureData{ "6_4.png", 0, { 0.84f, 0.2f }, { 0.08f, 0.0f }, { 0.42f, -0.3f }, COL_WHITE, { B1, 12 } },
                std::nullopt,
                std::nullopt, // glow
                new BlockHitboxData{ { 0.84f, 0.2f }, { 0.08f, 0.0f }, { 0.42f, -0.31f } },
                new BlockSpecialData{ SD_PAD, SD_BLUE },
            };
            blocks["6_6"] = {
                SPECIAL,
                new BlockTextureData{ "6_6.png", 0, { 1.0f, 1.0f }, { 0.0f, 0.0f }, { 0.5f, 0.5f }, COL_WHITE, { B1, 12 } },
                std::nullopt,
                std::nullopt, // glow
                new BlockHitboxData{ { 1.2f, 1.2f }, { -0.1f, -0.1f }, { 0.6f, 0.6f } },
                new BlockSpecialData{ SD_ORB, SD_YELLOW },
            };
            blocks["6_17"] = {
                SPECIAL,
                new BlockTextureData{ "6_17.png", 0, { 1.2f, 2.52f }, { -0.1f, -0.76f }, { 0.6f, 1.25f }, COL_WHITE, { T1, 10 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 0.84f, 2.52f }, { 0.08f, -0.76f }, { 0.42f, 1.25f } },
                new BlockSpecialData{ SD_PORTAL, SD_RIGHT_SIDE_UP },
            };
            blocks["6_18"] = {
                SPECIAL,
                new BlockTextureData{ "6_18.png", 0, { 1.2f, 2.52f }, { -0.1f, -0.76f }, { 0.6f, 1.25f }, COL_WHITE, { T1, 10 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 0.84f, 2.52f }, { 0.08f, -0.76f }, { 0.42f, 1.25f } },
                new BlockSpecialData{ SD_PORTAL, SD_UPSIDE_DOWN },
            };
            blocks["6_20"] = {
                SPECIAL,
                new BlockTextureData{ "6_20.png", 0, { 1.5f, 2.8f }, { -0.25f, -0.9f }, { 0.75f, 1.4f }, COL_WHITE, { T1, 10 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 1.14f, 2.84f }, { -0.07f, -0.92f }, { 0.57f, 1.41f } },
                new BlockSpecialData{ SD_PORTAL, SD_CUBE },
            };
            blocks["6_21"] = {
                SPECIAL,
                new BlockTextureData{ "6_21.png", 0, { 1.5f, 2.8f }, { -0.25f, -0.9f }, { 0.75f, 1.4f }, COL_WHITE, { T1, 10 } },
                std::nullopt,
                std::nullopt,
                new BlockHitboxData{ { 1.14f, 2.84f }, { -0.07f, -0.92f }, { 0.57f, 1.41f } },
                new BlockSpecialData{ SD_PORTAL, SD_SHIP },
            };

            blocks["11_1"] = {
                DECORATION,
                new BlockTextureData{ "11_1.png", 0, { 0.63f, 2.305f }, { 0.185f, -0.01f }, { 0.315f, 1.805f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["11_2"] = {
                DECORATION,
                new BlockTextureData{ "11_2.png", 0, { 0.63f, 1.11f }, { 0.185f, -0.01f }, { 0.315f, 0.61f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["11_193"] = {
                DECORATION,
                new BlockTextureData{ "11_193.png", 0, { 4.2f, 1.4f }, { -1.6f, -0.05f }, { 2.1f, 0.85f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["11_194"] = {
                DECORATION,
                new BlockTextureData{ "11_194.png", 0, { 3.6f, 1.15f }, { -1.3f, 0.0f }, { 1.8f, 0.65f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["11_195"] = {
                DECORATION,
                new BlockTextureData{ "11_195.png", 0, { 2.3f, 0.9f }, { -0.65f, 0.0f }, { 1.15f, 0.4f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["11_196"] = {
                DECORATION,
                new BlockTextureData{ "11_196.png", 0, { 1.4f, 0.4f }, { -0.2f, 0.05f }, { 0.7f, -0.05f }, COL_P_1, { B1, 9 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };

            blocks["12_77"] = {
                DECORATION,
                new BlockTextureData{ "12_77.png", 0, { 0.215f, 1.4f }, { 0.3925f, 0.0f }, { 0.1075f, 0.9f }, COL_WHITE, { B2, -6 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["12_78"] = {
                DECORATION,
                new BlockTextureData{ "12_78.png", 0, { 0.2f, 0.88f }, { 0.4f, 0.0f }, { 0.1f, 0.38f }, COL_WHITE, { B2, -6 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
            blocks["12_79"] = {
                DECORATION,
                new BlockTextureData{ "12_79.png", 0, { 0.175f, 0.405f }, { 0.4125f, 0.0f }, { 0.0875f, -0.095f }, COL_WHITE, { B2, -6 } },
                std::nullopt,
                std::nullopt,
                std::nullopt,
                std::nullopt,
            };
        
            triggers["13_1"] = { T_STARTPOS };
            triggers["13_2_1"] = { T_BACKGROUND };
            triggers["13_2_2"] = { T_GROUND };
            triggers["13_2_3"] = { T_LINE };
            triggers["13_108"] = { T_BE_FADE };
            triggers["13_109"] = { T_BE_FROM_TOP };
            triggers["13_110"] = { T_BE_FROM_BOTTOM };
            triggers["13_111"] = { T_BE_FROM_LEFT };
            triggers["13_112"] = { T_BE_FROM_RIGHT };
            triggers["13_113"] = { T_BE_SCALE };
        }

        ~Other() {
            for (const auto& pair : blocks) {
                if (pair.second.baseTexture.has_value()) delete pair.second.baseTexture.value();
                if (pair.second.detailTexture.has_value()) delete pair.second.detailTexture.value();
                if (pair.second.glow.has_value()) delete pair.second.glow.value();
                if (pair.second.hitboxData.has_value()) delete pair.second.hitboxData.value();
            }
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
