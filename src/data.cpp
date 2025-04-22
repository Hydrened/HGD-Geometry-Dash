#include "data.h"

// INIT
Data::Data(Game* g) : game(g) {
    engine = game->getEngine();

    loadBlocksData();

    initPhysics();
    initMenuObjects();
    initIcons();
}

void Data::loadBlocksData() {
    for (const json& block : H2DE_ReadJsonFile("data/items/blocks.gdd")) {
        BlockData bd = BlockData();
        bd.index = getIndex(getIndexFromJson(block["ind"]));

        for (const json& surface : block["sur"]) {
            BlockData::SurfaceData sd = BlockData::SurfaceData();
            sd.sheet = surface["sht"];
            sd.src = getAbsRectFromJson(surface["src"]);
            
            if (surface.contains("dst")) {
                sd.dest = getLevelRectFromJson(surface["dst"]);
            }

            if (surface.contains("col")) {
                sd.defaultColor = getColorFromJson(surface["col"]);
            }

            bd.surfaces.push_back(sd);
        }

        blocks[block["id"]] = bd;
    }
}

// physics
void Data::initPhysics() {
    initGravities();
    initMaxGravities();
    initRotations();
    initClicks();
    initPlayerHiboxes();
}

void Data::initGravities() {
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = -0.0244f;
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = -0.009f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initMaxGravities() {
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.4f;
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.22f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initRotations() {
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 6.4f;
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 6.4f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = -44.5f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = -44.5f;
}

void Data::initClicks() {
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.3332f;
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.23f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initPlayerHiboxes() {
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
}

// menus
void Data::initMenuObjects() {
    initMainMenuObjects();
    initLevelMenuObjects();
}

void Data::initMainMenuObjects() {
    H2DE_Hitbox levelMenuButtonHitbox = H2DE_Hitbox();
    levelMenuButtonHitbox.rect = { 0.0f, 0.0f, 3.65f, 3.65f };
    levelMenuButtonHitbox.color = { 255, 127, 127, 255 };

    H2DE_ObjectData levelMenuButtonOD = H2DE_ObjectData();
    levelMenuButtonOD.pos = { 8.27f, 3.54f };
    levelMenuButtonOD.size = levelMenuButtonHitbox.rect.getSize();
    levelMenuButtonOD.absolute = true;
    levelMenuButtonOD.hitboxes["click"] = levelMenuButtonHitbox;
    levelMenuButtonOD.index = getIndex(LAYER_UI, 0);

    H2DE_ButtonObjectData levelMenuButtonBOD = H2DE_ButtonObjectData();
    levelMenuButtonBOD.onMouseDown = [this](H2DE_Object* object) {
        
    };
    levelMenuButtonBOD.onMouseUp = [this](H2DE_Object* object) {
        callShortcut(SDLK_SPACE);
    };

    H2DE_SurfaceData levelMenuButtonSD = H2DE_SurfaceData();
    levelMenuButtonSD.textureName = "GJ_GameSheet04-uhd.png";
    levelMenuButtonSD.rect = H2DE_LevelPos{ 0.0f, 0.0f}.makeRect(levelMenuButtonHitbox.rect.getSize());
    levelMenuButtonSD.scaleMode = H2DE_SCALE_MODE_LINEAR;

    H2DE_TextureData levelMenuButtonTexD = H2DE_TextureData();
    levelMenuButtonTexD.srcRect = { 461, 78, 415, 415 };

    SurfaceDataBuffer levelMenuButtonSurfaceDataBuffer = SurfaceDataBuffer();
    levelMenuButtonSurfaceDataBuffer.surD = levelMenuButtonSD;
    levelMenuButtonSurfaceDataBuffer.texD = levelMenuButtonTexD;

    ButtonObjectBuffer levelMenuButtonBuffer = ButtonObjectBuffer();
    levelMenuButtonBuffer.bod = levelMenuButtonBOD;
    levelMenuButtonBuffer.sdb["test"] = levelMenuButtonSurfaceDataBuffer;

    ObjectBuffer levelMenuButton = ObjectBuffer();
    levelMenuButton.type = BUTTON_OBJECT;
    levelMenuButton.od = levelMenuButtonOD;
    levelMenuButton.buttonBuffer = levelMenuButtonBuffer;


    


    H2DE_ObjectData gameTitleOD = H2DE_ObjectData();
    gameTitleOD.pos = { 2.6f, 1.0f };
    gameTitleOD.size = { 15.0f, 1.7f };
    gameTitleOD.absolute = true;
    gameTitleOD.index = getIndex(LAYER_UI, 0);

    H2DE_SurfaceData gameTitleSD = H2DE_SurfaceData();
    gameTitleSD.textureName = "GJ_LaunchSheet-uhd.png";
    gameTitleSD.rect = H2DE_LevelPos{ 0.0f, 0.0f}.makeRect(gameTitleOD.size);
    gameTitleSD.scaleMode = H2DE_SCALE_MODE_LINEAR;

    H2DE_TextureData gameTitleTexD = H2DE_TextureData();
    gameTitleTexD.srcRect = { 197, 1, 1700, 192 };

    SurfaceDataBuffer gameTitleSurfaceDataBuffer = SurfaceDataBuffer();
    gameTitleSurfaceDataBuffer.surD = gameTitleSD;
    gameTitleSurfaceDataBuffer.texD = gameTitleTexD;

    BasicObjectBuffer gameTitleBuffer = BasicObjectBuffer();
    gameTitleBuffer.bod = H2DE_BasicObjectData();
    gameTitleBuffer.sdb["test"] = gameTitleSurfaceDataBuffer;

    ObjectBuffer gameTitle = ObjectBuffer();
    gameTitle.type = BASIC_OBJECT;
    gameTitle.od = gameTitleOD;
    gameTitle.basicBuffer = gameTitleBuffer;





    menuObjects[MAIN_MENU] = {
        levelMenuButton,
        gameTitle,
    };
}

void Data::initLevelMenuObjects() {
    menuObjects[LEVEL_MENU] = {

    };
}

// icons
void Data::initIcons() {
    initCubes();
}

void Data::initCubes() {
    IconData cube1 = IconData();
    cube1.col1SrcRect = { 31, 2, 120, 120 };
    cube1.col2SrcRect = { 2, 2, 26, 26 };
    cube1.glowSrcRect = { 3, 124, 128, 128 };
    cube1.col1DestRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    cube1.col2DestRect = { 0.3915f, 0.3915f, 0.217f, 0.217f };
    cube1.glowDestRect = { -0.035f, -0.035f, 1.07f, 1.07f };
    cubes[1] = cube1;
}

// CLEANUP
Data::~Data() {

}

// GETTER
// maps
template<typename T_Physic>
const T_Physic& Data::getPhysic(const std::unordered_map<PlayerGamemode, std::unordered_map<PlayerSize, T_Physic>>& physics, PlayerGamemode gamemode, PlayerSize size) {
    auto gIt = physics.find(gamemode);
    if (gIt == physics.end()) {
        throw std::runtime_error("HGD-403: Can't find gamemode id");
    }

    auto sIt = gIt->second.find(size);
    if (sIt == gIt->second.end()) {
        throw std::runtime_error("HGD-404: Can't find size id");
    }

    return sIt->second;
}

const float& Data::getGravity(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(gravities, gamemode, size);
}

const float& Data::getMaxGravity(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(maxGravities, gamemode, size);
}

const float& Data::getRotation(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(rotations, gamemode, size);
}

const float& Data::getClick(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(clicks, gamemode, size);
}

const std::pair<H2DE_LevelRect, H2DE_LevelRect>& Data::getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(playerHitboxes, gamemode, size);
}

const std::vector<ObjectBuffer>& Data::getMenuObjects(MenuID id) const {
    auto it = menuObjects.find(id);
    if (it == menuObjects.end()) {
        throw std::runtime_error("HGD-401: Can't find menu id");
    }

    return it->second;
}

const IconData& Data::getCube(int id) const {
    auto it = cubes.find(id);
    if (it == cubes.end()) {
        throw std::runtime_error("HGD-402: Can't find cube id");
    }

    return it->second;
}

const BlockData& Data::getBlock(const std::string& id) const {
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        throw std::runtime_error("HGD-406: Can't find block id");
    }

    return it->second;
}

// vectors
const float& Data::getSpeed(int speed) const {
    return speeds[speed];
}

const int Data::getSpeed(float vx) const {
    auto it = std::find(speeds.begin(), speeds.end(), vx);
    if (it == speeds.end()) {
        throw std::runtime_error("HGD-405: Can't find x velocity");
    }

    return static_cast<int>(it - speeds.begin());
}

// positions
const H2DE_LevelPos& Data::getDefaultCameraPos() const {
    return defaultCameraPos;
}

const H2DE_LevelPos& Data::getDefaultBackgroundPos() const {
    return defaultBackgroundPos;
}

const H2DE_LevelPos& Data::getDefaultBotGroundPos() const {
    return defaultBotGroundPos;
}

const H2DE_LevelPos& Data::getDefaultTopGroundPos() const {
    return defaultTopGroundPos;
}

// sizes
const H2DE_LevelSize& Data::getGameSize() const {
    return gameSize;
}

const H2DE_LevelSize& Data::getBackgroundTileSize() const {
    return backgroundTileSize;
}

const H2DE_LevelSize& Data::getGroundTileSize() const {
    return groundTileSize;
}

const H2DE_LevelSize& Data::getGroundSize() const {
    return groundSize;
}

const H2DE_LevelSize& Data::getLineSize() const {
    return lineSize;
}

// colors
const H2DE_ColorRGB& Data::getDefaultBackgroundColor() const {
    return defaultBackgroundColor;
}

const H2DE_ColorRGB& Data::getDefaultGroundColor() const {
    return defaultGroundColor;
}

const H2DE_ColorRGB& Data::getDefaultLineColor() const {
    return defaultLineColor;
}

// other
const int& Data::getGroundMovementDuration() const {
    return groundMovementDuration;
}

const float& Data::getLineOffsetX() const {
    return lineOffsetX;
}
