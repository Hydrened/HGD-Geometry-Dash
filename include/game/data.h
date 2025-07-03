#ifndef DATA_H
#define DATA_H

#include "game.h"
class Game;

class Data {
public:
    enum MenuObjectBufferType {
        OBJECT_BUFFER_TYPE_BASIC,
        OBJECT_BUFFER_TYPE_BUTTON,
        OBJECT_BUFFER_TYPE_TEXT,
    };

    struct SurfaceBuffer {
        H2DE_SurfaceData surface = H2DE_SurfaceData();
        H2DE_TextureData texture = H2DE_TextureData();
    };

    struct SurfaceBuffers {
        std::vector<SurfaceBuffer> normals = {};
        std::unordered_map<int, std::vector<SurfaceBuffer>> randoms = {};
    };

    struct MenuObjectBuffer {
        MenuObjectBufferType type;
        H2DE_ObjectData objectData;
        std::optional<H2DE_ButtonObjectData> button = std::nullopt;
        std::optional<H2DE_TextObjectData> text = std::nullopt;
        std::vector<SurfaceBuffer> surfaces = {};
        std::vector<H2DE_Hitbox> hitboxes = {};
    };

    struct ModalButtonBuffer {
        std::string text = "";
        SDL_Keycode shortcut = SDLK_ESCAPE;
        std::function<void()> callback = nullptr;
    };

    struct ModalBuffer {
        std::string title = "";
        std::string message = "";
        std::vector<ModalButtonBuffer> buttons = {};
    };

    struct IconSurfaceBuffer {
        std::string textureName = "";
        H2DE_PixelRect src = H2DE_PixelRect();
        H2DE_LevelRect dest = H2DE_LevelRect();
        float rotation = 0.0f;
        H2DE_ColorRGB color = H2DE_ColorRGB();
        int index = 0;
    };

    struct BlockBuffer {
        BlockType type = BLOCK_TYPE_DECORATION;
        SurfaceBuffers surfaces = SurfaceBuffers();
        std::optional<H2DE_Hitbox> hitbox = std::nullopt;
        int index = 0;
    };

public:
    Data(Game* game);
    ~Data() = default;

    const Data::SurfaceBuffer& getSurfaceBuffer(const std::string& name) const;
    const Data::SurfaceBuffer getBackgroundTileSurfaceBuffer(uint8_t backgroundID, float translateX) const;
    const Data::SurfaceBuffer getGroundTileSurfaceBuffer(uint8_t groundID, float translateX) const;
    const std::vector<Data::MenuObjectBuffer>& getMenuObjects(MenuID id) const;
    const Data::ModalBuffer& getModalBuffer(ModalID id) const;
    const Data::BlockBuffer& getBlockBuffer(const std::string& id) const;
    const std::vector<Data::IconSurfaceBuffer> getIconSurfacesBuffer(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const;
    const float& getGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getMaxGravity(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getClick(PlayerGamemode gamemode, PlayerSize size) const;
    const float& getRotation(PlayerGamemode gamemode, PlayerSize size) const;
    const PlayerHitbox& getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const;
    const PlayerSnap& getPlayerSnap(PlayerGamemode gamemode, PlayerSize size) const;
    const uint8_t& getNbIcons(PlayerGamemode gamemode) const;

    const float& getSpeedVelocityX(Speed speed) const;
    const Speed getSpeedFromVelocityX(float vx) const;
    const H2DE_ColorRGB& getHitboxColor(const std::string& color) const;
    const H2DE_ColorRGB& getIconColor(Color_ID id) const;

    const PlayerIcons getRandomPlayerIcons() const;
    const std::tuple<Speed, PlayerGamemode, PlayerSize> getRandomPlayerState() const;

    constexpr const H2DE_Translate& getDefaultCameraTranslate() const { return defaultCameraTranslate; }
    constexpr const H2DE_Translate& getLevelMenuCameraTranslate() const { return levelMenuCameraTranslate; }
    constexpr const H2DE_Translate& getDefaultBackgroundTranslate() const { return defaultBackgroundTranslate; }
    constexpr const H2DE_Translate& getDefaultBotGroundTranslate() const { return defaultBotGroundTranslate; }
    constexpr const H2DE_Translate& getDefaultTopGroundTranslate() const { return defaultTopGroundTranslate; }
    constexpr const H2DE_Translate& getIconMenuMainIconTranslate() const { return iconMenuMainIconTranslate; }
    constexpr const H2DE_Translate& getIconMenuFirstIconButtonTranslate() const { return iconMenuFirstIconButtonTranslate; }
    constexpr const H2DE_Translate& getIconMenuFirstCol1ButtonTranslate() const { return iconMenuFirstCol1ButtonTranslate; }
    constexpr const H2DE_Translate& getIconMenuFirstCol2ButtonTranslate() const { return iconMenuFirstCol2ButtonTranslate; }

    constexpr const H2DE_Scale& getBackgroundScale() const { return backgroundScale; }
    constexpr const H2DE_Scale& getGroundScale() const { return groundScale; }
    constexpr const H2DE_Scale& getLineScale() const { return lineScale; }
    constexpr const H2DE_Scale& getBackgroundTileScale() const { return backgroundTileScale; }
    constexpr const H2DE_Scale& getGroundTileScale() const { return groundTileScale; }
    constexpr const H2DE_Scale& getIconMenuIconButtonScale() const { return iconMenuIconButtonScale; }
    constexpr const H2DE_Scale& getIconMenuColorButtonScale() const { return iconMenuColorButtonScale; }
    constexpr const H2DE_Scale& getBlockScale() const { return blockScale; }

    constexpr const H2DE_ColorRGB& getDefaultBackgroundColor() const { return defaultBackgroundColor; }
    constexpr const H2DE_ColorRGB& getDefaultGroundColor() const { return defaultGroundColor; }
    constexpr const H2DE_ColorRGB& getDefaultLineColor() const { return defaultLineColor; }

    constexpr const float& getMinCameraY() const { return minCameraY; }
    constexpr const float& getMiniPlayerRatio() const { return miniPlayerRatio; }
    constexpr const float& getIconMenuMainIconScaleMultiplier() const { return iconMenuMainIconScaleMultiplier; }
    constexpr const float& getIconMenuButtonSpacing() const { return iconMenuButtonSpacing; }
    constexpr const float& getCameraItemPadding() const { return cameraItemPadding; }
    constexpr const float& getCamearaOffsetXFromPlayer() const { return camearaOffsetXFromPlayer; }

    constexpr const int& getIconMenuMainIconOrder() const { return iconMenuMainIconOrder; }
    constexpr const int& getIconMenuButtonsOrder() const { return iconMenuButtonsOrder; }
    constexpr const uint8_t getNbIconColors() const { return iconsColors.size(); }
    constexpr const uint32_t& getStartingLevelDelayDuration() const { return startingLevelDelayDuration; }

    inline static const Speed getLevelSpeed(const json& levelData) {
        return static_cast<Speed>(levelData["config"]["speed"]);
    }
    inline static const uint8_t getLevelBackgroundID(const json& levelData) {
        return static_cast<uint8_t>(levelData["config"]["background"]["id"]);
    }
    inline static const uint8_t getLevelGroundID(const json& levelData) {
        return static_cast<uint8_t>(levelData["config"]["ground"]["id"]);
    }
    inline static PlayerGamemode getLevelPlayerGamemode(const json& levelData) {
        return static_cast<PlayerGamemode>(static_cast<int>(levelData["config"]["gamemode"]));
    }
    inline static PlayerSize getLevelPlayerSize(const json& levelData) {
        return static_cast<PlayerSize>(static_cast<int>(levelData["config"]["size"]));
    }
    inline static PlayerGravity getLevelPlayerGravity(const json& levelData) {
        return static_cast<PlayerGravity>(static_cast<int>(levelData["config"]["gravity"]));
    }
    inline static std::string getLevelSong(const json& levelData) {
        return levelData["config"]["song"];
    }

    inline static BlockType getBlockType(const json& blockData) {
        return static_cast<BlockType>(static_cast<int>(blockData["type"]));
    }
    inline static int getBlockIndex(const json& blockData) {
        return Data::getIndex(static_cast<Layer>(static_cast<int>(blockData["index"]["layer"])), static_cast<int>(blockData["index"]["order"]));
    }

    constexpr static const int getIndex(Layer layer, int order) {
        constexpr int RANGE = 100;
        return layer * RANGE * 2 + layer + std::clamp(order, -RANGE, RANGE) + RANGE;
    }

    const H2DE_ButtonObjectData getDefaultButtonObjectData(const std::function<void()>& onMouseUp) const;

    void createStretchedTextureFromSquare(H2DE_BasicObject* basicObject, const std::string& squareTextureName, const H2DE_Scale& scale) const;

private:
    Game* game;

    // MAPS
    std::unordered_map<MenuID, std::vector<Data::MenuObjectBuffer>> menuObjects = {};
    std::unordered_map<ModalID, Data::ModalBuffer> modalBuffers = {};
    std::unordered_map<std::string, Data::BlockBuffer> blockBuffers = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<Icon_ID, std::vector<Data::IconSurfaceBuffer>>> iconSurfacesBuffers = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> gravities = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> maxGravities = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> clicks = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, float>> rotations = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, PlayerHitbox>> playerHitboxes = {};
    std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, PlayerSnap>> playerSnaps = {};
    std::unordered_map<PlayerGamemode, uint8_t> icons = { { PLAYER_GAMEMODE_CUBE, 13u }, { PLAYER_GAMEMODE_SHIP, 1u } };



    // ARRAYS
    const std::array<float, 4> speeds = { 0.1395334f, 0.1731001f, 0.2152334f, 0.2600001f }; // speed 4 = 0.3200001f
    const std::array<PlayerGamemode, 2> gamemodes = { PLAYER_GAMEMODE_CUBE, PLAYER_GAMEMODE_SHIP };
    const std::array<PlayerSize, 2> sizes = { PLAYER_SIZE_NORMAL, PLAYER_SIZE_MINI };
    const std::array<H2DE_ColorRGB, 3> hitboxesColors = { H2DE_ColorRGB{ 255, 127, 127, 255 }, { 127, 255, 127, 255 }, { 127, 127, 255, 255 } };
    const std::array<H2DE_ColorRGB, 12> iconsColors = {
        H2DE_ColorRGB{ 125, 255, 0, 255 }, { 0, 255, 0, 255 }, { 0, 255, 125, 255 }, { 0, 255, 255, 255 },
        { 0, 125, 255, 255 }, { 0, 0, 255, 255 }, { 125, 0, 255, 255 }, { 255, 0, 255, 255 },
        { 255, 0, 125, 255 }, { 255, 0, 0, 255 }, { 255, 125, 0, 255 }, { 255, 225, 0, 255 }
    };



    // SRC RECTS
    const H2DE_PixelRect lineTextureSrcRect = { 1, 13, 1803, 6 };
    const H2DE_PixelRect grayLockerSrcRect = { 3002, 1908, 86, 104 };

    // -- main menu
    const H2DE_PixelRect mainMenuGameTitleTextureSrcRect = { 197, 1, 1700, 192 };
    const H2DE_PixelRect mainMenuLevelButtonTextureSrcRect = { 461, 78, 415, 415 };
    const H2DE_PixelRect mainMenuCreatorButtonTextureSrcRect = { 803, 1359, 264, 264 };
    const H2DE_PixelRect mainMenuIconsButtonTextureSrcRect = { 960, 1676, 264, 264 };
    const H2DE_PixelRect mainMenuQuitGameButtonTextureSrcRect = { 2489, 2235, 182, 179 };
    const H2DE_PixelRect iconeMenuSelectedButtonTextureSrcRect = { 861, 2684, 144, 144 };

    // -- all menus
    const H2DE_PixelRect menuCornerTextureSrcRect = { 2803, 1517, 285, 282 };
    const H2DE_PixelRect menuExitArrowTextureSrcRect = { 1395, 2817, 149, 124 };
    const H2DE_PixelRect menuTapForMoreInfoTextureSrcRect = { 1504, 180, 71, 422 };


    
    // TRANSLATES
    const H2DE_Translate defaultCameraTranslate = { 2.5f, 1.84375f };
    const H2DE_Translate levelMenuCameraTranslate = { 2.5f, 3.16065f };
    const H2DE_Translate defaultBackgroundTranslate = { 2.5f, 5.0f };
    const H2DE_Translate defaultBotGroundTranslate = { 2.5f, -2.5f };
    const H2DE_Translate defaultTopGroundTranslate = { 2.5f, 77.5f };

    // -- main menu
    const H2DE_Translate mainMenuGameTitleTranslate = { 0.059f, 3.622f };
    const H2DE_Translate mainMenuLevelButtonTranslate = { 0.079f, 0.247f };
    const H2DE_Translate mainMenuCreatorButtonTranslate = { 3.731f, 0.277f };
    const H2DE_Translate mainMenuIconsButtonTranslate = { -3.622f, 0.277f };
    const H2DE_Translate mainMenuQuitGameButtonTranslate = { -8.886f, 4.72f };

    // -- icon menu
    const H2DE_Translate iconMenuBackgroundTranslate = { 0.0f, 0.0f };
    const H2DE_Translate iconMenuTopLeftCornerTranslate = { -8.322f, 4.186f };
    const H2DE_Translate iconMenuTopRightCornerTranslate = { 8.322f, 4.186f };
    const H2DE_Translate iconMenuExitArrowTranslate = { -8.689f, 4.542f };
    const H2DE_Translate iconMenuLineTranslate = { 0.0f, 0.336f };
    const H2DE_Translate iconMenuIconBackgroundTranslate = { 0.0f, -1.168f };
    const H2DE_Translate iconMenuTapForMoreInfoTranslate = { 5.581f, -0.208f };
    const H2DE_Translate iconMenuMainIconTranslate = { 0.0f, 1.17f };
    const H2DE_Translate iconMenuFirstIconButtonTranslate = { -7.224f, -1.168f };
    const H2DE_Translate iconMenuFirstCol1ButtonTranslate = { -6.630f, -3.137f };
    const H2DE_Translate iconMenuFirstCol2ButtonTranslate = { -6.630f, -4.335f };



    // OFFETS
    const H2DE_Translate lineOffset = { 0.0f, 1.975f };
    const H2DE_Translate iconMenuSelectedButtonOffset = { 0.015f, -0.015f };
    


    // SCALES
    const H2DE_Scale backgroundScale = { 19.0f, 19.0f };
    const H2DE_Scale groundScale = { 27.0f, 4.0f };
    const H2DE_Scale lineScale = { 13.0f, 0.05f };
    const H2DE_Scale backgroundTileScale = { 19.0f, 19.0f };
    const H2DE_Scale groundTileScale = { 4.0f, 4.0f };
    const H2DE_Scale grayLockerScale = { 0.762f, 0.891f };
    const H2DE_Scale blockScale = { 1.0f, 1.0f };

    // -- main menu
    const H2DE_Scale mainMenuGameTitleScale = { 14.191f, 1.613f };
    const H2DE_Scale mainMenuLevelButtonScale = { 3.464f, 3.464f };
    const H2DE_Scale mainMenuCreatorButtonScale = { 2.197f, 2.197f };
    const H2DE_Scale mainMenuIconsButtonScale = { 2.197f, 2.197f };
    const H2DE_Scale mainMenuQuitGameButtonScale = { 1.049f, 1.049f };

    // -- icon menu
    const H2DE_Scale iconMenuBackgroundScale = { 19.0f, 10.6875f };
    const H2DE_Scale iconMenuIconBackgroundScale = { 16.061f, 1.851f };
    const H2DE_Scale iconMenuIconButtonScale = { 1.0f, 1.0f };
    const H2DE_Scale iconMenuColorButtonScale = { 1.0f, 1.0f };
    const H2DE_Scale iconMenuSelectedButtonScale = { 1.2f, 1.2f };
    
    // -- all menus
    const H2DE_Scale menuCornerScale = { 2.355f, 2.326f };
    const H2DE_Scale menuExitArrowScale = { 1.247f, 1.059f };
    const H2DE_Scale menuTapForMoreInfoScale = { 0.594f, 3.563f };

    // -- modal
    const H2DE_Scale modalCornerScale = { 1.6f, 1.6f };



    // COLORS
    const H2DE_ColorRGB defaultBackgroundColor = { 10, 97, 255, 255 };
    const H2DE_ColorRGB defaultGroundColor = { 8, 97, 252, 255 };
    const H2DE_ColorRGB defaultLineColor = { 255, 255, 255, 255 };
    const H2DE_ColorRGB grayIconCol1 = { 175, 175, 175, 255 };
    const H2DE_ColorRGB grayIconCol2 = { 255, 255, 255, 255 };



    // FLOATERS
    const float minCameraY = 1.84375f;
    const float buttonPressedScaleMultiplier = 1.3f;
    const float miniPlayerRatio = 0.629f;
    const float cubeInVehiculeRatio = 0.56f;
    const float cubeInVehiculeOffsetY = 0.18f;
    const float iconMenuMainIconScaleMultiplier = 1.613f;
    const float iconMenuButtonSpacing = 0.198f;
    const float cameraItemPadding = 5.0f;
    const float camearaOffsetXFromPlayer = 3.0f;



    // INTEGERS
    const int iconMenuMainIconOrder = 3;
    const int iconMenuButtonsOrder = 3;
    const uint32_t startingLevelDelayDuration = 1000;



    // SURFACES
    const H2DE_SurfaceData backgroundTile_surfaceData = Data::makeSurfaceData(backgroundTileScale, 0);
    const H2DE_SurfaceData groundTile_surfaceData = Data::makeSurfaceData(groundTileScale, 0);
    const H2DE_SurfaceData line_surfaceData = Data::makeSurfaceData(lineOffset, lineScale, 1);
    const H2DE_SurfaceData grayLocker_surfaceData = Data::makeSurfaceData(grayLockerScale, 1);

    // -- main menu
    const H2DE_SurfaceData mainMenuGameTitle_surfaceData = Data::makeSurfaceData(mainMenuGameTitleScale);
    const H2DE_SurfaceData mainMenuLevelButton_surfaceData = Data::makeSurfaceData(mainMenuLevelButtonScale);
    const H2DE_SurfaceData mainMenuCreatorButton_surfaceData = Data::makeSurfaceData(mainMenuCreatorButtonScale);
    const H2DE_SurfaceData mainMenuIconsButton_surfaceData = Data::makeSurfaceData(mainMenuIconsButtonScale);
    const H2DE_SurfaceData mainMenuQuitGameButton_surfaceData = Data::makeSurfaceData(mainMenuQuitGameButtonScale, 270.0f);

    // -- icon menu
    const H2DE_SurfaceData iconMenuBackground_surfaceData = Data::makeSurfaceData(iconMenuBackgroundScale);
    const H2DE_SurfaceData iconMenuTopLeftCorner_surfaceData = Data::makeSurfaceData(menuCornerScale, 90.0f);
    const H2DE_SurfaceData iconMenuTopRightCorner_surfaceData = Data::makeSurfaceData(menuCornerScale, 180.0f);
    const H2DE_SurfaceData iconMenuExitArrow_surfaceData = Data::makeSurfaceData(menuExitArrowScale, 270.0f);
    const H2DE_SurfaceData iconMenuLine_surfaceData = Data::makeSurfaceData(lineScale);
    const H2DE_SurfaceData iconMenuIconBackground_surfaceData = Data::makeSurfaceData(iconMenuIconBackgroundScale);
    const H2DE_SurfaceData iconMenuTapForMoreInfo_surfaceData = Data::makeSurfaceData(menuTapForMoreInfoScale, 270.0f);
    const H2DE_SurfaceData iconMenuSelectedButton_surfaceData = Data::makeSurfaceData(iconMenuSelectedButtonOffset, iconMenuSelectedButtonScale);



    // TEXTURES
    const H2DE_TextureData backgroundTile_textureData = Data::makeTextureData("game_bg_[ID]_001-uhd.png");
    const H2DE_TextureData groundTile_textureData = Data::makeTextureData("groundSquare_[ID]_001-uhd.png");
    const H2DE_TextureData line_textureData = Data::makeTextureData("GJ_GameSheet02-uhd.png", lineTextureSrcRect);
    const H2DE_TextureData grayLocker_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", grayLockerSrcRect);

    // -- main menu
    const H2DE_TextureData mainMenuGameTitle_textureData = Data::makeTextureData("GJ_LaunchSheet-uhd.png", mainMenuGameTitleTextureSrcRect);
    const H2DE_TextureData mainMenuLevelButton_textureData = Data::makeTextureData("GJ_GameSheet04-uhd.png", mainMenuLevelButtonTextureSrcRect);
    const H2DE_TextureData mainMenuCreatorButton_textureData = Data::makeTextureData("GJ_GameSheet04-uhd.png", mainMenuCreatorButtonTextureSrcRect);
    const H2DE_TextureData mainMenuIconsButton_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", mainMenuIconsButtonTextureSrcRect);
    const H2DE_TextureData mainMenuQuitGameButton_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", mainMenuQuitGameButtonTextureSrcRect);

    // -- icon menu
    const H2DE_TextureData iconMenuBackground_textureData = Data::makeTextureData("icon-menu-bg.png");
    const H2DE_TextureData iconMenuLine_textureData = Data::makeTextureData("GJ_GameSheet02-uhd.png", lineTextureSrcRect);
    const H2DE_TextureData iconMenuIconBackground_textureData = Data::makeTextureData("icon-menu-icon-background.png");
    const H2DE_TextureData iconMenuTapForMoreInfo_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", menuTapForMoreInfoTextureSrcRect);
    const H2DE_TextureData iconMenuSelectedButton_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", iconeMenuSelectedButtonTextureSrcRect);

    // -- all menus
    const H2DE_TextureData menuCorner_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", menuCornerTextureSrcRect);
    const H2DE_TextureData menuExitArrow_textureData = Data::makeTextureData("GJ_GameSheet03-uhd.png", menuExitArrowTextureSrcRect);



    // HITBOXES
    const H2DE_Hitbox mainMenuLevelButton_hitbox = Data::makeHitbox(mainMenuLevelButtonScale, getHitboxColor("red"));
    const H2DE_Hitbox mainMenuCreatorButton_hitbox = Data::makeHitbox(mainMenuCreatorButtonScale, getHitboxColor("red"));
    const H2DE_Hitbox mainMenuIconsButton_hitbox = Data::makeHitbox(mainMenuIconsButtonScale, getHitboxColor("red"));
    const H2DE_Hitbox mainMenuQuitGameButton_hitbox = Data::makeHitbox(mainMenuQuitGameButtonScale, getHitboxColor("red"));

    // -- all menus
    const H2DE_Hitbox exitArrowButton_hitbox = Data::makeHitbox({ menuExitArrowScale.y, menuExitArrowScale.x }, getHitboxColor("red"));



    // SURFACE BUFFERS
    const std::unordered_map<std::string, Data::SurfaceBuffer> surfaceBuffers = {
        { "line", { line_surfaceData, line_textureData } },

        { "gray-locker", { grayLocker_surfaceData, grayLocker_textureData } },

        { "main-menu-game-title", { mainMenuGameTitle_surfaceData, mainMenuGameTitle_textureData } },
        { "main-menu-level-button", { mainMenuLevelButton_surfaceData, mainMenuLevelButton_textureData } },
        { "main-menu-creator-button", { mainMenuCreatorButton_surfaceData, mainMenuCreatorButton_textureData } },
        { "main-menu-icons-button", { mainMenuIconsButton_surfaceData, mainMenuIconsButton_textureData } },
        { "main-menu-quit-game-button", { mainMenuQuitGameButton_surfaceData, mainMenuQuitGameButton_textureData } },

        { "icon-menu-background", { iconMenuBackground_surfaceData, iconMenuBackground_textureData } },
        { "icon-menu-top-left-corner", { iconMenuTopLeftCorner_surfaceData, menuCorner_textureData } },
        { "icon-menu-top-right-corner", { iconMenuTopRightCorner_surfaceData, menuCorner_textureData } },
        { "icon-menu-exit-arrow", { iconMenuExitArrow_surfaceData, menuExitArrow_textureData } },
        { "icon-menu-line", { iconMenuLine_surfaceData, iconMenuLine_textureData } },
        { "icon-menu-icon-background", { iconMenuIconBackground_surfaceData, iconMenuIconBackground_textureData } },
        { "icon-menu-tap-for-more-info", { iconMenuTapForMoreInfo_surfaceData, iconMenuTapForMoreInfo_textureData } },
        { "icon-menu-selected-button", { iconMenuSelectedButton_surfaceData, iconMenuSelectedButton_textureData } },
    };



    void initMenuObjects();
    void initMainMenuObjects();
    void initLevelMenuObjects();
    void initIconMenuObjects();

    void initMenuBasicObject(MenuID id, const H2DE_Translate& translate, const std::string& surfaceBuffer, int index);
    void initMenuButtonObject(MenuID id, const H2DE_Translate& translate, const std::string& surfaceBuffer, int index, const H2DE_Hitbox& hitbox, const std::function<void()>& onMouseUp = nullptr);

    void initModalBuffers();
    void initQuitGameModalBuffer();

    void initIconSurfacesBuffers();
    void initCubeSurfacesBuffers();
    void initShipSurfacesBuffers();

    void initBlockBuffers();
    Data::BlockBuffer initBlockBuffer(const json& blockData) const;
    const Data::SurfaceBuffers initBlockBufferSurfaces(const json& blockData) const;
    const H2DE_Hitbox initBlockBufferHitbox(const json& blockData, BlockType type) const;

    void initPhysics();
    void initGravities();
    void initMaxGravities();
    void initClicks();
    void initRotations();
    void initPlayerHitboxes();
    void initPlayerSnaps();

    inline static const H2DE_SurfaceData makeSurfaceData(const H2DE_Scale& scale) { return Data::makeSurfaceData({ 0.0f, 0.0f }, scale, 0.0f, 0); }
    inline static const H2DE_SurfaceData makeSurfaceData(const H2DE_Scale& scale, float rotation) { return Data::makeSurfaceData({ 0.0f, 0.0f }, scale, rotation, 0); }
    inline static const H2DE_SurfaceData makeSurfaceData(const H2DE_Scale& scale, int index) { return Data::makeSurfaceData({ 0.0f, 0.0f }, scale, 0.0f, index); }
    inline static const H2DE_SurfaceData makeSurfaceData(const H2DE_Translate& translate, const H2DE_Scale& scale) { return Data::makeSurfaceData(translate, scale, 0.0f, 0); }
    inline static const H2DE_SurfaceData makeSurfaceData(const H2DE_Translate& translate, const H2DE_Scale& scale, int index) { return Data::makeSurfaceData(translate, scale, 0.0f, index); }
    static const H2DE_SurfaceData makeSurfaceData(const H2DE_Translate& translate, const H2DE_Scale& scale, float rotation, int index);

    inline static const H2DE_TextureData makeTextureData(const std::string& textureName) { return Data::makeTextureData(textureName, std::nullopt); }
    static const H2DE_TextureData makeTextureData(const std::string& textureName, const std::optional<H2DE_PixelRect>& srcRect);

    static const H2DE_Hitbox makeHitbox(const H2DE_Scale& scale, const H2DE_ColorRGB& color);

    static const Data::SurfaceBuffer getTileSurfaceBuffer(Data::SurfaceBuffer& surfaceBuffer, uint8_t id, float translateX);
    void getIconSurfacesBufferCheckID(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons) const;
    const std::vector<Data::IconSurfaceBuffer> getIconSurfacesBufferGetAllSurfaces(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const;
    const std::vector<Data::IconSurfaceBuffer> getIconSurfacesBufferGetFilteredSurfaces(const std::vector<Data::IconSurfaceBuffer>& allSurfaces, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const;

    template<typename Physic_T>
    static const Physic_T& getPhysic(const std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, Physic_T>>& physics, PlayerGamemode gamemode, PlayerSize size);
};

#endif
