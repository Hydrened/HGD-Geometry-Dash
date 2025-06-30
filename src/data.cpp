#include "data.h"

// INIT
Data::Data(Game* g) : game(g) {
    initMenuObjects();
    initModalBuffers();
    initIconSurfacesBuffers();
    initBlockBuffers();
    initPhysics();
}

// -- make structs
const H2DE_SurfaceData Data::makeSurfaceData(const H2DE_Translate& translate, const H2DE_Scale& scale, float rotation, int index) {
    H2DE_SurfaceData res = H2DE_SurfaceData();
    res.transform.translate = translate;
    res.transform.scale = scale;
    res.transform.rotation = rotation;
    res.index = index;
    return res;
}

const H2DE_TextureData Data::makeTextureData(const std::string& textureName, const std::optional<H2DE_PixelRect>& srcRect) {
    H2DE_TextureData res = H2DE_TextureData();
    res.textureName = textureName;
    res.srcRect = srcRect;
    return res;
}

const H2DE_Hitbox Data::makeHitbox(const H2DE_Scale& scale, const H2DE_ColorRGB& color) {
    H2DE_Hitbox res = H2DE_Hitbox();
    res.transform.scale = scale;
    res.color = color;
    return res;
}

// -- menu objects
void Data::initMenuObjects() {
    initMainMenuObjects();
    initLevelMenuObjects();
    initIconMenuObjects();
}

void Data::initMenuBasicObject(MenuID id, const H2DE_Translate& translate, const std::string& surfaceBuffer, int index) {
    MenuObjectBuffer objectBuffer = MenuObjectBuffer();
    objectBuffer.type = OBJECT_BUFFER_TYPE_BASIC;

    objectBuffer.objectData.transform.translate = translate;
    objectBuffer.objectData.absolute = true;
    objectBuffer.objectData.index = Data::getIndex(LAYER_UI, index);
    objectBuffer.surfaces = { getSurfaceBuffer(surfaceBuffer) };

    menuObjects[id].push_back(objectBuffer);
}

void Data::initMenuButtonObject(MenuID id, const H2DE_Translate& translate, const std::string& surfaceBuffer, int index, const H2DE_Hitbox& hitbox, const std::function<void()>& onMouseUp) {
    MenuObjectBuffer objectBuffer = MenuObjectBuffer();
    objectBuffer.type = OBJECT_BUFFER_TYPE_BUTTON;

    objectBuffer.objectData.transform.translate = translate;
    objectBuffer.objectData.absolute = true;
    objectBuffer.objectData.index = Data::getIndex(LAYER_UI, index);
    objectBuffer.surfaces = { getSurfaceBuffer(surfaceBuffer) };
    objectBuffer.hitboxes = { hitbox };
    objectBuffer.button = getDefaultButtonObjectData(onMouseUp);

    menuObjects[id].push_back(objectBuffer);
}

// -- -- main menu
void Data::initMainMenuObjects() {
    menuObjects[MENU_ID_MAIN_MENU] = {};

    initMenuBasicObject(MENU_ID_MAIN_MENU, mainMenuGameTitleTranslate, "main-menu-game-title", 0);

    initMenuButtonObject(MENU_ID_MAIN_MENU, mainMenuLevelButtonTranslate, "main-menu-level-button", 0, mainMenuLevelButton_hitbox, []() {
        callShortcut(SDLK_SPACE);
    });
    initMenuButtonObject(MENU_ID_MAIN_MENU, mainMenuCreatorButtonTranslate, "main-menu-creator-button", 0, mainMenuCreatorButton_hitbox, 0);
    initMenuButtonObject(MENU_ID_MAIN_MENU, mainMenuIconsButtonTranslate, "main-menu-icons-button", 0, mainMenuIconsButton_hitbox, []() {
        callShortcut(SDLK_i);
    });
    initMenuButtonObject(MENU_ID_MAIN_MENU, mainMenuQuitGameButtonTranslate, "main-menu-quit-game-button", 0, mainMenuQuitGameButton_hitbox, []() {
        callShortcut(SDLK_ESCAPE);
    });
}

// -- -- level menu
void Data::initLevelMenuObjects() {
    menuObjects[MENU_ID_LEVEL_MENU] = {};
}

// -- -- icon menu
void Data::initIconMenuObjects() {
    menuObjects[MENU_ID_ICON_MENU] = {};

    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuBackgroundTranslate, "icon-menu-background", 0);

    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuTopLeftCornerTranslate, "icon-menu-top-left-corner", 1);
    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuTopRightCornerTranslate, "icon-menu-top-right-corner", 1);
    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuLineTranslate, "icon-menu-line", 1);

    initMenuButtonObject(MENU_ID_ICON_MENU, iconMenuExitArrowTranslate, "icon-menu-exit-arrow", 2, exitArrowButton_hitbox, []() {
        callShortcut(SDLK_ESCAPE);
    });

    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuIconBackgroundTranslate, "icon-menu-icon-background", 1);
    initMenuBasicObject(MENU_ID_ICON_MENU, iconMenuTapForMoreInfoTranslate, "icon-menu-tap-for-more-info", 2);
}

// -- modals
void Data::initModalBuffers() {
    initQuitGameModalBuffer();
}

void Data::initQuitGameModalBuffer() {
    modalBuffers[MODAL_ID_QUIT_GAME] = {
        "Quit Game",
        "Are you sure you want to quit ?",
        {
            { "Cancel", SDLK_ESCAPE, nullptr},
            { "Yes", SDLK_SPACE, [this]() {
                game->stop();
            }},
        }
    };
}

// -- surfaces
void Data::initIconSurfacesBuffers() {
    initCubeSurfacesBuffers();
    initShipSurfacesBuffers();
}

void Data::initCubeSurfacesBuffers() {
    std::vector<IconSurfaceBuffer> cube_0 = {};
    cube_0.push_back({ "player_00-uhd.png", { 137, 3, 120, 120 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f, H2DE_ColorRGB(), 2 });
    cube_0.push_back({ "player_00-uhd.png", { 256, 3, 43, 43 }, { 0.0f, 0.0f, 0.35833f, 0.35833f }, 0.0f, H2DE_ColorRGB(), 1 });
    cube_0.push_back({ "player_00-uhd.png", { 3, 2, 132, 132 }, { 0.0f, 0.0f, 1.06667f, 1.06667f }, 0.0f, H2DE_ColorRGB(), 0 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_CUBE][0] = cube_0;

    std::vector<IconSurfaceBuffer> cube_1 = {};
    cube_1.push_back({ "player_01-uhd.png", { 31, 2, 120, 120 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f, H2DE_ColorRGB(), 2 });
    cube_1.push_back({ "player_01-uhd.png", { 2, 2, 26, 26 }, { 0.0f, 0.0f, 0.21667f, 0.21667f }, 0.0f, H2DE_ColorRGB(), 1 });
    cube_1.push_back({ "player_01-uhd.png", { 3, 124, 128, 128 }, { 0.0f, 0.0f, 1.06667f, 1.06667f }, 0.0f, H2DE_ColorRGB(), 0 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_CUBE][1] = cube_1;

    std::vector<IconSurfaceBuffer> cube_2 = {};
    cube_2.push_back({ "player_02-uhd.png", { 133, 2, 120, 120 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f, H2DE_ColorRGB(), 2 });
    cube_2.push_back({ "player_02-uhd.png", { 132, 126, 77, 58 }, { 0.0f, 0.0f, 0.64167f, 0.48333f }, 0.0f, H2DE_ColorRGB(), 1 });
    cube_2.push_back({ "player_02-uhd.png", { 3, 2, 128, 128 }, { 0.0f, 0.0f, 1.06667f, 1.06667f }, 0.0f, H2DE_ColorRGB(), 0 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_CUBE][2] = cube_2;

    std::vector<IconSurfaceBuffer> cube_3 = {};
    cube_3.push_back({ "player_03-uhd.png", { 133, 2, 120, 120 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f, H2DE_ColorRGB(), 2 });
    cube_3.push_back({ "player_03-uhd.png", { 132, 125, 112, 65 }, { 0.175f, 0.0f, 0.93333f, 0.54167f }, 270.0f, H2DE_ColorRGB(), 1 });
    cube_3.push_back({ "player_03-uhd.png", { 2, 3, 128, 128 }, { 0.0f, 0.0f, 1.06667f, 1.06667f }, 270.0f, H2DE_ColorRGB(), 0 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_CUBE][3] = cube_3;

    std::vector<IconSurfaceBuffer> cube_4 = {};
    cube_4.push_back({ "player_04-uhd.png", { 133, 2, 120, 120 }, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.0f, H2DE_ColorRGB(), 2 });
    cube_4.push_back({ "player_04-uhd.png", { 133, 124, 72, 53 }, { 0.0f, 0.06f, 0.6f, 0.44167f }, 0.0f, H2DE_ColorRGB(), 1 });
    cube_4.push_back({ "player_04-uhd.png", { 2, 3, 128, 128 }, { 0.0f, 0.0f, 1.06667f, 1.06667f }, 0.0f, H2DE_ColorRGB(), 0 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_CUBE][4] = cube_4;
}

void Data::initShipSurfacesBuffers() {
    std::vector<IconSurfaceBuffer> ship_1 = {};
    ship_1.push_back({ "ship_01-uhd.png", { 205, 3, 149, 87 }, { 0.0f, -0.16f, 1.24167f, 0.725f }, 0.0f, H2DE_ColorRGB(), 4 });
    ship_1.push_back({ "ship_01-uhd.png", { 163, 5, 39, 93 }, { 0.04f, 0.01f, 0.325f, 0.775f }, 270.0f, H2DE_ColorRGB(), 3 });
    ship_1.push_back({ "ship_01-uhd.png", { 3, 3, 156, 95 }, { 0.0f, -0.16f, 1.3f, 0.79167f }, 0.0f, H2DE_ColorRGB(), -1 });
    iconSurfacesBuffers[PLAYER_GAMEMODE_SHIP][1] = ship_1;
}

// -- blocks
void Data::initBlockBuffers() {
    json blocks = H2DE_Json::read("data/items/blocks.gdd");

    for (const json& block : blocks) {
        if (!block.contains("id")) {
            throw std::runtime_error("Block has no id");
        }

        if (!block.contains("type")) {
            throw std::runtime_error("Block has no type");
        }

        if (!block.contains("index")) {
            throw std::runtime_error("Block has no index");
        }

        blockBuffers[block["id"]] = initBlockBuffer(block);
    }
}

Data::BlockBuffer Data::initBlockBuffer(const json& blockData) const {
    Data::BlockBuffer res = Data::BlockBuffer();

    res.type = Data::getBlockType(blockData);
    res.index = Data::getBlockIndex(blockData);

    if (blockData.contains("surfaces")) {
        res.surfaces = initBlockBufferSurfaces(blockData);
    }

    if (blockData.contains("hitbox")) {
        res.hitbox = initBlockBufferHitbox(blockData, res.type);
    }

    return res;
}

const Data::SurfaceBuffers Data::initBlockBufferSurfaces(const json& blockData) const {
    SurfaceBuffers res = SurfaceBuffers();

    const std::string blockID = blockData["id"];
    const std::vector<json>& surfaces = blockData["surfaces"];
    res.normals.reserve(surfaces.size());

    for (const json& surface : surfaces) {

        if (!surface.contains("sheet")) {
            throw std::runtime_error("Surface from block id \"" + blockID + "\" has no sheet");
        }

        if (!surface.contains("src")) {
            throw std::runtime_error("Surface from block id \"" + blockID + "\" has no src");
        }

        if (!surface.contains("dest")) {
            throw std::runtime_error("Surface from block id \"" + blockID + "\" has no dest");
        }

        if (!surface.contains("index")) {
            throw std::runtime_error("Surface from block id \"" + blockID + "\" has no index");
        }

        Data::SurfaceBuffer surfaceBuffer = Data::SurfaceBuffer();

        const H2DE_LevelRect dest = H2DE_Json::getRect<float>(surface["dest"]);
        surfaceBuffer.surface.transform.translate = dest.getTranslate();
        surfaceBuffer.surface.transform.scale = dest.getScale();
        surfaceBuffer.surface.index = H2DE_Json::getInteger(surface["index"]);

        surfaceBuffer.texture.textureName = surface["sheet"];
        surfaceBuffer.texture.srcRect = H2DE_Json::getRect<int>(surface["src"]);

        if (surface.contains("rotation")) {
            surfaceBuffer.surface.transform.rotation = H2DE_Json::getFloat(surface["rotation"]);
        }

        if (surface.contains("color")) {
            surfaceBuffer.texture.color = H2DE_Json::getColorRGB(surface["color"], false);
        }

        if (surface.contains("random-id")) {

            int randomID = H2DE_Json::getInteger(surface["random-id"]);

            auto it = res.randoms.find(randomID);
            if (it == res.randoms.end()) {
                res.randoms[randomID] = { surfaceBuffer };
            } else {
                res.randoms[randomID].push_back(surfaceBuffer);
            }

        } else {
            res.normals.push_back(surfaceBuffer);
        }
    }

    return res;
}

const H2DE_Hitbox Data::initBlockBufferHitbox(const json& blockData, BlockType type) const {
    H2DE_Hitbox res = H2DE_Hitbox();

    const H2DE_LevelRect hitboxRect = H2DE_Json::getRect<float>(blockData["hitbox"]);
    res.transform.translate = hitboxRect.getTranslate();
    res.transform.scale = hitboxRect.getScale();

    const std::string strColor = (type == BLOCK_TYPE_OBSTACLE) ? "red" : (type == BLOCK_TYPE_SOLID) ? "blue" : "green";
    res.color = getHitboxColor(strColor);

    return res;
}

// -- physics
void Data::initPhysics() {
    initGravities();
    initMaxGravities();
    initClicks();
    initRotations();
    initPlayerHitboxes();
    initPlayerSnaps();
}

void Data::initGravities() {
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.026784f;
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.026784f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.026784f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.026784f;
}

void Data::initMaxGravities() {
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.4f;
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.4f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.22f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.22f;
}

void Data::initClicks() {
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.329348f;
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.2f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.2f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.2f;
}

void Data::initRotations() {
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 6.9f;
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 6.9f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = -44.5f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = -44.5f;
}

void Data::initPlayerHitboxes() {
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.35f, 0.35f } };
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 0.629f, 0.629f }, { 0.0f, 0.0f, 0.22015f, 0.22015f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.35f, 0.35f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 0.629f, 0.629f }, { 0.0f, 0.0f, 0.22015f, 0.22015f } };
}

void Data::initPlayerSnaps() {
    playerSnaps[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = { true, false };
    playerSnaps[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = { true, false };
    playerSnaps[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = { true, true };
    playerSnaps[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = { true, true };
}

// ACTIONS
void Data::createStretchedTextureFromSquare(H2DE_BasicObject* basicObject, const std::string& squareTextureName, const H2DE_Scale& scale) const {
    const H2DE_PixelSize textureSize = game->getEngine()->getTextureSize(squareTextureName);

    const H2DE_PixelSize halfTexture = {
        static_cast<int>(H2DE::round(textureSize.x * 0.5f)),
        static_cast<int>(H2DE::round(textureSize.y * 0.5f))
    };


    
    const std::array<std::pair<H2DE_Translate, H2DE_PixelPos>, 4> corners = {
        std::pair<H2DE_Translate, H2DE_PixelPos>{ { -1.0f, 1.0f }, { 0, 0 } },
        std::pair<H2DE_Translate, H2DE_PixelPos>{ { 1.0f, 1.0f }, { halfTexture.x, 0 } },
        std::pair<H2DE_Translate, H2DE_PixelPos>{ { 1.0f, -1.0f }, halfTexture },
        std::pair<H2DE_Translate, H2DE_PixelPos>{ { -1.0f, -1.0f }, { 0, halfTexture.y } }
    };

    for (int i = 0; i < corners.size(); i++) {
        const auto& [multiplier, srcPos] = corners[i];

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.transform.scale = modalCornerScale;

        H2DE_TextureData td = H2DE_TextureData();
        td.textureName = squareTextureName;
        td.srcRect = srcPos.makeRect(halfTexture);
        sd.transform.translate.x = scale.x * 0.5f * multiplier.x;
        sd.transform.translate.y = scale.y * 0.5f * multiplier.y;

        basicObject->addSurface<H2DE_Texture>("background-corner-" + std::to_string(i), sd, td);
    }



    const std::array<std::tuple<H2DE_Translate, H2DE_Scale, H2DE_PixelRect>, 4> borderDatas = {
        std::tuple<H2DE_Translate, H2DE_Scale, H2DE_PixelRect>{ { 0.0f, 0.5f }, { scale.x, modalCornerScale.y }, { halfTexture.x, 0, 1, halfTexture.y } },
        std::tuple<H2DE_Translate, H2DE_Scale, H2DE_PixelRect>{ { 0.5f, 0.0f }, { modalCornerScale.x, scale.y }, { halfTexture.x, halfTexture.y, halfTexture.x, 1 } },
        std::tuple<H2DE_Translate, H2DE_Scale, H2DE_PixelRect>{ { 0.0f, -0.5f }, { scale.x, modalCornerScale.y }, { halfTexture.x, halfTexture.y, 1, halfTexture.y } },
        std::tuple<H2DE_Translate, H2DE_Scale, H2DE_PixelRect>{ { -0.5f, 0.0f }, { modalCornerScale.x, scale.y }, { 0, halfTexture.y, halfTexture.x, 1 } }
    };
    
    for (int i = 0; i < borderDatas.size(); i++) {
        const auto& [tra, sca, src] = borderDatas[i];

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.transform.translate.x = scale.x * tra.x;
        sd.transform.translate.y = scale.y * tra.y;
        sd.transform.scale = sca;

        H2DE_TextureData td = H2DE_TextureData();
        td.textureName = squareTextureName;
        td.srcRect = src;

        basicObject->addSurface<H2DE_Texture>("background-border-" + std::to_string(i), sd, td);
    }



    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.transform.scale = scale;

    H2DE_TextureData td = H2DE_TextureData();
    td.textureName = squareTextureName;
    td.srcRect = halfTexture.makeRect({ 1, 1 });

    basicObject->addSurface<H2DE_Texture>("background-center", sd, td);
}

// GETTERS

// -- maps
const Data::SurfaceBuffer& Data::getSurfaceBuffer(const std::string& name) const {
    auto it = surfaceBuffers.find(name);
    if (it == surfaceBuffers.end()) {
        throw std::runtime_error("Could not find surface buffer named \"" + name + "\"");    
    }

    return it->second;
}

const Data::SurfaceBuffer Data::getTileSurfaceBuffer(Data::SurfaceBuffer& surfaceBuffer, uint8_t id, float translateX) {
    surfaceBuffer.surface.transform.translate.x = translateX;

    std::ostringstream stringifiedID;
    stringifiedID << std::setw(2) << std::setfill('0') << std::to_string(id);

    size_t pos = surfaceBuffer.texture.textureName.find("[ID]");
    if (pos != std::string::npos) {
        surfaceBuffer.texture.textureName.replace(pos, 4, stringifiedID.str());
    }

    return surfaceBuffer;
}

const Data::SurfaceBuffer Data::getBackgroundTileSurfaceBuffer(uint8_t backgroundID, float translateX) const {
    Data::SurfaceBuffer res = Data::SurfaceBuffer();
    res.surface = backgroundTile_surfaceData;
    res.texture = backgroundTile_textureData;

    return getTileSurfaceBuffer(res, backgroundID, translateX);
}

const Data::SurfaceBuffer Data::getGroundTileSurfaceBuffer(uint8_t groundID, float translateX) const {
    Data::SurfaceBuffer res = Data::SurfaceBuffer();
    res.surface = groundTile_surfaceData;
    res.texture = groundTile_textureData;

    return getTileSurfaceBuffer(res, groundID, translateX);
}

const std::vector<Data::MenuObjectBuffer>& Data::getMenuObjects(MenuID id) const {
    auto it = menuObjects.find(id);
    if (it == menuObjects.end()) {
        throw std::runtime_error("Could not find the menu id \"" + std::to_string(id) + "\"");
    }

    return it->second;
}

const Data::ModalBuffer& Data::getModalBuffer(ModalID id) const {
    auto it = modalBuffers.find(id);
    if (it == modalBuffers.end()) {
        throw std::runtime_error("Could not find the modal id \"" + std::to_string(id) + "\"");
    }

    return it->second;
}

const Data::BlockBuffer& Data::getBlockBuffer(const std::string& id) const {
    auto it = blockBuffers.find(id);
    if (it == blockBuffers.end()) {
        throw std::runtime_error("Could not find the block id \"" + id + "\"");
    }

    return it->second;
}

const std::vector<Data::IconSurfaceBuffer> Data::getIconSurfacesBuffer(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const {
    getIconSurfacesBufferCheckID(gamemode, size, playerIcons);
    const std::vector<Data::IconSurfaceBuffer> allSurfaces = getIconSurfacesBufferGetAllSurfaces(gamemode, size, playerIcons, gray);
    return getIconSurfacesBufferGetFilteredSurfaces(allSurfaces, size, playerIcons, gray);
}

void Data::getIconSurfacesBufferCheckID(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons) const {
    auto gamemode_it = iconSurfacesBuffers.find(gamemode);
    if (gamemode_it == iconSurfacesBuffers.end()) {
        throw std::runtime_error("Could not find gamemode \"" + std::to_string(gamemode) + "\"");
    }

    bool isCube = (gamemode == PLAYER_GAMEMODE_CUBE);
    bool isMini = (size == PLAYER_SIZE_MINI);

    Icon_ID mainIconID = (isCube) ? playerIcons.cubeID : playerIcons.shipID;

    if (isCube && isMini) {
        mainIconID = 0;
    }

    auto& id_map = gamemode_it->second;
    auto id_it = id_map.find(mainIconID);
    if (id_it == id_map.end()) {
        throw std::runtime_error("Could not find id \"" + std::to_string(static_cast<int>(mainIconID)) + "\"");
    }
}

const std::vector<Data::IconSurfaceBuffer> Data::getIconSurfacesBufferGetAllSurfaces(PlayerGamemode gamemode, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const {
    bool isCube = (gamemode == PLAYER_GAMEMODE_CUBE);
    bool isMini = (size == PLAYER_SIZE_MINI);

    Icon_ID mainIconID = (isCube) ? playerIcons.cubeID : playerIcons.shipID;

    if (isCube && isMini) {
        mainIconID = 0;
    }
    
    std::vector<Data::IconSurfaceBuffer> res = iconSurfacesBuffers.at(gamemode).at(mainIconID);

    bool gamemodeHasCubeInIt = (gamemode == PLAYER_GAMEMODE_SHIP);
    if (gamemodeHasCubeInIt) {

        PlayerIcons inVehiculeIcons = playerIcons;
        if (isMini) {
            inVehiculeIcons.cubeID = 0;
        }

        std::vector<Data::IconSurfaceBuffer> littleCubeSurfacesBuffer = getIconSurfacesBuffer(PLAYER_GAMEMODE_CUBE, PLAYER_SIZE_NORMAL, inVehiculeIcons, gray);

        for (Data::IconSurfaceBuffer& littleCubeSurfaceBuffer : littleCubeSurfacesBuffer) {
            littleCubeSurfaceBuffer.dest *= cubeInVehiculeRatio;
            littleCubeSurfaceBuffer.dest.y += cubeInVehiculeOffsetY;
        }

        res.insert(res.end(), littleCubeSurfacesBuffer.begin(), littleCubeSurfacesBuffer.end());
    }
    
    return res;
}

const std::vector<Data::IconSurfaceBuffer> Data::getIconSurfacesBufferGetFilteredSurfaces(const std::vector<Data::IconSurfaceBuffer>& allSurfaces, PlayerSize size, const PlayerIcons& playerIcons, bool gray) const {
    std::vector<Data::IconSurfaceBuffer> res = {};

    bool isMini = (size == PLAYER_SIZE_MINI);

    for (size_t i = 0; i < allSurfaces.size(); i++) {
        IconSurfaceBuffer surfaceBuffer = allSurfaces[i];

        if (i < 6) {
            bool isGlow = (i % 3 == 2);
            if (isGlow && (!playerIcons.glow || gray)) {
                continue;
            }

            surfaceBuffer.color = (gray)
                ? (i % 3 == 0) ? grayIconCol1 : grayIconCol2
                : (i % 3 == 0) ? getIconColor(playerIcons.col1) : getIconColor(playerIcons.col2);
        }

        if (isMini) {
            surfaceBuffer.dest.y *= miniPlayerRatio;
            surfaceBuffer.dest.w *= miniPlayerRatio;
            surfaceBuffer.dest.h *= miniPlayerRatio;
        }

        res.push_back(surfaceBuffer);
    }

    return res;
}

template<typename Physic_T>
const Physic_T& Data::getPhysic(const std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, Physic_T>>& physics, PlayerGamemode gamemode, PlayerSize size) {
    auto gamemode_it = physics.find(gamemode);
    if (gamemode_it == physics.end()) {
        throw std::runtime_error("Could not find gamemode \"" + std::to_string(gamemode) + "\"");
    }

    auto size_it = gamemode_it->second.find(size);
    if (size_it == gamemode_it->second.end()) {
        throw std::runtime_error("Could not find size \"" + std::to_string(size) + "\"");
    }

    return size_it->second;
}

const float& Data::getGravity(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(gravities, gamemode, size);
}

const float& Data::getMaxGravity(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(maxGravities, gamemode, size);
}

const float& Data::getClick(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(clicks, gamemode, size);
}

const float& Data::getRotation(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(rotations, gamemode, size);
}

const PlayerHitbox& Data::getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(playerHitboxes, gamemode, size);
}

const PlayerSnap& Data::getPlayerSnap(PlayerGamemode gamemode, PlayerSize size) const {
    return Data::getPhysic(playerSnaps, gamemode, size);
}

const uint8_t& Data::getNbIcons(PlayerGamemode gamemode) const {
    auto it = icons.find(gamemode);
    if (it == icons.end()) {
        throw std::runtime_error("Could not find gamemode \"" + std::to_string(gamemode) + "\"");
    }

    return it->second;
}

// -- arrays
const float& Data::getSpeedVelocityX(Speed speed) const {
    if (speed >= speeds.size()) {
        throw std::runtime_error("Could not find speed \"" + std::to_string(speed) + "\"");    
    }

    return speeds[speed];
}

const Speed Data::getSpeedFromVelocityX(float vx) const {
    constexpr float EPSILON = 0.001f;

    for (size_t i = 0; i < speeds.size(); ++i) {
        if (H2DE::abs(speeds[i] - vx) < EPSILON) {
            return static_cast<Speed>(i);
        }
    }

    throw std::runtime_error("Could not find velocity \"" + std::to_string(vx) + "\"");
}

const H2DE_ColorRGB& Data::getHitboxColor(const std::string& color) const {
    static const std::array<std::string, 3> colors = { "red", "green", "blue" };

    auto it = std::find(colors.begin(), colors.end(), color);
    if (it == colors.end()) {
        throw std::runtime_error("Could not find hitbox color named \"" + color + "\"");    
    }

    size_t index = static_cast<size_t>(it - colors.begin());
    return hitboxesColors[index];
}

const H2DE_ColorRGB& Data::getIconColor(Color_ID id) const {
    if (id >= iconsColors.size()) {
        throw std::runtime_error("Could not find color id \"" + std::to_string(static_cast<int>(id)) + "\"");    
    }

    return iconsColors[id];
}

// -- others
const H2DE_ButtonObjectData Data::getDefaultButtonObjectData(const std::function<void()>& onMouseUp) const {
    H2DE_ButtonObjectData buttonData = H2DE_ButtonObjectData();

    buttonData.onMouseDown = [this](H2DE_ButtonObject* button, H2DE_TimelineID& timelineID) {
        button->stopTimeline();

        timelineID = button->setScale(H2DE_Scale{ 1.0f, 1.0f } * buttonPressedScaleMultiplier, 350, H2DE_EASING_BOUNCE_OUT, [button, &timelineID]() {
            timelineID = H2DE_INVALID_TIMELINE_ID;
        }, false);
    };

    buttonData.onMouseUp = [onMouseUp](H2DE_ButtonObject* button, H2DE_TimelineID& timelineID) {
        button->stopTimeline();

        button->setScale({ 1.0f, 1.0f });

        if (onMouseUp) {
            onMouseUp();
        }
    };

    buttonData.onBlur = [](H2DE_ButtonObject* button, H2DE_TimelineID& timelineID) {
        button->stopTimeline();

        timelineID = button->setScale({ 1.0f, 1.0f }, 500, H2DE_EASING_BOUNCE_OUT, [button, &timelineID]() {
            timelineID = H2DE_INVALID_TIMELINE_ID;
        }, false);
    };

    return buttonData;
}

const PlayerIcons Data::getRandomPlayerIcons() const {
    PlayerIcons res = PlayerIcons();

    res.col1 = H2DE::randomIntegerInRange(0, iconsColors.size() - 1);
    res.col2 = H2DE::randomIntegerInRange(0, iconsColors.size() - 1);
    res.cubeID = H2DE::randomIntegerInRange(1, iconSurfacesBuffers.at(PLAYER_GAMEMODE_CUBE).size() - 1);
    res.shipID = H2DE::randomIntegerInRange(1, iconSurfacesBuffers.at(PLAYER_GAMEMODE_SHIP).size());
    res.glow = H2DE::randomBool();

    return res;
}

const std::tuple<Speed, PlayerGamemode, PlayerSize> Data::getRandomPlayerState() const {
    return {
        1, // H2DE::randomIntegerInRange(1, speeds.size() - 1) en 1.7
        static_cast<PlayerGamemode>(H2DE::randomIntegerInRange(0, gamemodes.size() - 1)),
        PLAYER_SIZE_NORMAL // static_cast<PlayerSize>(H2DE::randomIntegerInRange(0, sizes.size() - 1)) // en 1.4
    };
}
