#include "data.h"

// INIT
Data::Data(Game* g) : game(g) {
    engine = game->getEngine();

    initHitboxesColors();
    loadItemsData();

    initPhysics();
    initMenuObjects();
    initIcons();
}

void Data::loadItemsData() {
    loadBlocksData();
    loadTriggersData();
}

void Data::loadBlocksData() {
    for (const json& block : H2DE_ReadJsonFile("data/items/blocks.gdd")) {
        BlockData bd = BlockData();

        if (!block.contains("id")) {
            std::cerr << "WARNING: Found block with no ID" << std::endl;
            continue;
        }

        const std::string id = H2DE_GetStringFromJson(block["id"]);

        if (!block.contains("surfaces")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no surfaces" << std::endl;
            continue;
        }

        if (!block.contains("index")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no index" << std::endl;
            continue;
        }

        if (!block.contains("type")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no type" << std::endl;
            continue;
        }

        const BlockType type = static_cast<BlockType>(block["type"]);

        bd.index = getIndex(getIndexFromJson(block["index"]));
        bd.surfaces = Data::getBlockSurfacesDatas(block, id);
        bd.type = type;

        if (block.contains("hitbox")) {
            BlockData::HitboxData hitboxData = BlockData::HitboxData();
            hitboxData.dest = H2DE_GetRectFromJson<float>(block["hitbox"]);
            hitboxData.color = getHitboxColor(type);
            bd.hitbox = hitboxData;
        }

        blockDatas[id] = bd;
    }
}

void Data::loadTriggersData() {
    for (const json& trigger : H2DE_ReadJsonFile("data/items/triggers.gdd")) {
        TriggerData td = TriggerData();

        if (!trigger.contains("id")) {
            std::cerr << "WARNING: Found trigger with no ID" << std::endl;
            continue;
        }

        const std::string id = H2DE_GetStringFromJson(trigger["id"]);

        if (!trigger.contains("type")) {
            std::cerr << "ERROR: Trigger ID: \"" << id << "\" has no type" << std::endl;
            continue;
        }

        const TriggerType type = static_cast<TriggerType>(trigger["type"]);

        td.type = type;

        triggerDatas[id] = td;
    }
}

const std::vector<std::vector<BlockData::SurfaceData>> Data::getBlockSurfacesDatas(const json& block, const std::string& id) {
    std::vector<std::vector<BlockData::SurfaceData>> res = {};

    if (block["surfaces"].size() == 0) {
        std::cerr << "WARNING: Block ID: \"" << id << "\" has 0 surfaces" << std::endl;
    }

    std::unordered_map<int, std::vector<BlockData::SurfaceData>> randomIds = {};

    for (const json& surface : block["surfaces"]) {
        BlockData::SurfaceData sd = BlockData::SurfaceData();

        if (!surface.contains("sheet")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no a surface with no sheet" << std::endl;
            continue;
        }

        if (!surface.contains("src")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no a surface with no src" << std::endl;
            continue;
        }
        
        if (!surface.contains("dest")) {
            std::cerr << "ERROR: Block ID: \"" << id << "\" has no a surface with no dest" << std::endl;
            continue;
        }

        sd.sheet = surface["sheet"];
        sd.src = H2DE_GetRectFromJson<int>(surface["src"]);
        sd.dest = H2DE_GetRectFromJson<float>(surface["dest"]);

        if (surface.contains("color")) {
            sd.defaultColor = H2DE_GetColorRgbFromJson(surface["color"], false);
        }

        if (surface.contains("rotation")) {
            sd.rotation = H2DE_GetFloatFromJson(surface["rotation"]);
            sd.dest = Data::rotateRect<float>(sd.dest, sd.rotation);
        }
        
        if (surface.contains("random-id")) {
            int randomID = surface["random-id"];
            auto it = randomIds.find(randomID);

            if (it == randomIds.end()) {
                randomIds[randomID] = { sd };
            } else {
                randomIds[randomID].push_back(sd);
            }
        } else {
            res.push_back({ sd });
        }
    }

    for (const auto& [randomID, sd] : randomIds) {
        res.push_back(sd);
    }

    return res;
}

// physics
void Data::initPhysics() {
    initGravities();
    initMaxGravities();
    initRotations();
    initClicks();
    initPlayerHiboxes();
    initPlayerSnaps();
}

void Data::initGravities() {
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.026784f;
    gravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.0f;
    gravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initMaxGravities() {
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.4f;
    maxGravities[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.22f;
    maxGravities[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initRotations() {
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 6.9f;
    rotations[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 6.9f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = -44.5f;
    rotations[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = -44.5f;
}

void Data::initClicks() {
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = 0.329348f;
    clicks[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = 0.0f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = 0.0f;
    clicks[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = 0.0f;
}

void Data::initPlayerHiboxes() {
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.325f, 0.325f, 0.35f, 0.35f } };
    playerHitboxes[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.325f, 0.325f, 0.35f, 0.35f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.325f, 0.325f, 0.35f, 0.35f } };
    playerHitboxes[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = { { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.325f, 0.325f, 0.35f, 0.35f } };
}

void Data::initPlayerSnaps() {
    playerSnaps[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_NORMAL] = { true, false };
    playerSnaps[PLAYER_GAMEMODE_CUBE][PLAYER_SIZE_MINI] = { true, false };
    playerSnaps[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_NORMAL] = { true, true };
    playerSnaps[PLAYER_GAMEMODE_SHIP][PLAYER_SIZE_MINI] = { true, true };
}

// menus
void Data::initMenuObjects() {
    initMainMenuObjects();
    initLevelMenuObjects();
}

void Data::initMainMenuObjects() {
    const H2DE_LevelRect levelMenuButtonDestRect = { 7.675f, 3.54f, 3.65f, 3.65f };

    H2DE_Hitbox levelMenuButtonHitbox = H2DE_Hitbox();
    levelMenuButtonHitbox.rect = H2DE_LevelPos{ 0.0f, 0.0f }.makeRect(levelMenuButtonDestRect.getSize());
    levelMenuButtonHitbox.color.a = 0;

    H2DE_ObjectData levelMenuButtonOD = H2DE_ObjectData();
    levelMenuButtonOD.rect = levelMenuButtonDestRect;
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


    

    const H2DE_LevelRect gameTitleDestRect = { 2.0f, 1.0f, 15.0f, 1.7f };

    H2DE_ObjectData gameTitleOD = H2DE_ObjectData();
    gameTitleOD.rect = gameTitleDestRect;
    gameTitleOD.absolute = true;
    gameTitleOD.index = getIndex(LAYER_UI, 0);

    H2DE_SurfaceData gameTitleSD = H2DE_SurfaceData();
    gameTitleSD.textureName = "GJ_LaunchSheet-uhd.png";
    gameTitleSD.rect = H2DE_LevelPos{ 0.0f, 0.0f}.makeRect(gameTitleDestRect.getSize());
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

// other
void Data::initHitboxesColors() {
    hitboxesColors = {
        { "red", { 255, 127, 127, 255 } },
        { "green", { 127, 255, 127, 255 } },
        { "blue", { 127, 127, 255, 255 } },
    };
}

void Data::initDefaultCheckpoint() {
    defaultCheckpoint.pos = { 0.0f, 0.0f };
    defaultCheckpoint.speed = 1;
    defaultCheckpoint.velocityY = 0.0f;
    defaultCheckpoint.rotation = 0.0f;

    defaultCheckpoint.gamemode = PLAYER_GAMEMODE_CUBE;
    defaultCheckpoint.size = PLAYER_SIZE_NORMAL;
    defaultCheckpoint.gravity = PLAYER_GRAVITY_NORMAL;
    
    defaultCheckpoint.gamemodeEntryPosY = std::nullopt;
    defaultCheckpoint.camPosY = defaultCameraPos.y;
}

// CLEANUP
Data::~Data() {

}

// GETTER
// utils
template<typename H2DE_Rect_T>
H2DE_Rect<H2DE_Rect_T> Data::rotateRect(const H2DE_Rect<H2DE_Rect_T>& rect, float rotation) {
    while (rotation < 0.0f) {
        rotation += 360.0f;
    }
    while (rotation >= 360.0f) {
        rotation -= 360.0f;
    }

    rotation = Data::snapRotation(rotation, 90.0f);

    if (rotation == 90.0f || rotation == 270.0f) {
        const float cx = rect.x + rect.w / 2.0f;
        const float cy = rect.y + rect.h / 2.0f;

        return H2DE_Rect<H2DE_Rect_T> { 
            static_cast<H2DE_Rect_T>(cx - rect.h / 2.0f),
            static_cast<H2DE_Rect_T>(cy - rect.w / 2.0f),
            rect.h,
            rect.w,
        };

    } else {
        return rect;
    }
}

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

const PlayerHitbox& Data::getPlayerHitbox(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(playerHitboxes, gamemode, size);
}

const PlayerSnap& Data::getPlayerSnap(PlayerGamemode gamemode, PlayerSize size) const {
    return getPhysic(playerSnaps, gamemode, size);
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

const BlockData Data::getBlockData(const std::string& id, const json& block) const {
    auto it = blockDatas.find(id);
    if (it == blockDatas.end()) {
        throw std::runtime_error("HGD-406: Can't find block id \"" + id + "\"");
    }

    BlockData blockData = it->second;
    blockData.rotation = block.contains("r") ? H2DE_GetFloatFromJson(block["r"]) : 0.0f;
    blockData.flip = block.contains("f") ? static_cast<H2DE_Flip>(block["r"]) : H2DE_FLIP_NONE;
    
    return blockData;
}

const TriggerData Data::getTriggerData(const std::string& id, const json& trigger) const {
    auto it = triggerDatas.find(id);
    if (it == triggerDatas.end()) {
        throw std::runtime_error("HGD-407: Can't find trigger id \"" + id + "\"");
    }

    TriggerData triggerData = it->second;
    triggerData.duration = trigger.contains("d") ? H2DE_GetIntFromJson(trigger["d"]) : 0;
    triggerData.color = trigger.contains("c") ? H2DE_GetColorRgbFromJson(trigger["c"], false) : H2DE_ColorRGB{ 255, 255, 255, 255 };
    triggerData.touchTrigger = trigger.contains("tt") ? H2DE_GetBoolFromJson(trigger["tt"]) : false;

    return triggerData;
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

const H2DE_ColorRGB& Data::getHitboxColor(const std::string& color) const {
    auto it = hitboxesColors.find(color);
    if (it == hitboxesColors.end()) {
        throw std::runtime_error("HGD-8: Can't find color: " + color);
    }

    return it->second;
}

const H2DE_ColorRGB& Data::getHitboxColor(BlockType blockType) const {
    std::string strColor = "none";

    switch (blockType) {
        case BLOCK_TYPE_SOLID: strColor = "blue"; break;
        case BLOCK_TYPE_OBSTACLE: strColor = "red"; break;
        case BLOCK_TYPE_SPECIAL: strColor = "green"; break;
        default: break;
    }

    return getHitboxColor(strColor);
}
 