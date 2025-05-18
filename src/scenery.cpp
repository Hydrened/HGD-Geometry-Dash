#include "scenery.h"

// INIT
Scenery::Scenery(Game* g, int bgID, int gID) : game(g), backgroundID(bgID), groundID(gID) {
    initDefaultValues();

    createGrounds();
    createLines();

    addMissingTiles();
    update();
}

void Scenery::initDefaultValues() {
    static const Data* data = game->getData();
    
    static const H2DE_LevelPos& defaultBackgroundPos = data->getDefaultBackgroundPos();
    static const H2DE_LevelPos& defaultBotGroundPos = data->getDefaultBotGroundPos();
    static const H2DE_LevelPos& defaultTopGroundPos = data->getDefaultTopGroundPos();

    displayedBackgroundPosY = defaultBackgroundPos.y;
    collisionBotGroundPosY = defaultBotGroundPos.y;
    displayedBotGroundPosY = collisionBotGroundPosY;
    collisionTopGroundPosY = defaultTopGroundPos.y;
    displayedTopGroundPosY = collisionTopGroundPosY;

    static const H2DE_ColorRGB& defaultGroundColor =  data->getDefaultGroundColor();
    static const H2DE_ColorRGB& defaultLineColor =  data->getDefaultLineColor();
    static const H2DE_ColorRGB& defaultBackgroundColor =  data->getDefaultBackgroundColor();

    backgroundColor = defaultBackgroundColor;
    groundColor = defaultGroundColor;
    lineColor = defaultLineColor;

    static H2DE_Engine* engine = game->getEngine();
    static const float groundTileWidth = data->getGroundTileSize().x;
    static const float backgroundTileWidth = data->getBackgroundTileSize().x;
    static const float camWidth = H2DE_GetCameraSize(engine).x;

    backgroundTiles.reserve((camWidth / backgroundTileWidth) + 1);
    botGroundTiles.reserve((camWidth / groundTileWidth) + 1);
    topGroundTiles.reserve((camWidth / groundTileWidth) + 1);
    lines.reserve(2);
}

void Scenery::createGrounds() {
    botGround = createGround(BOT_GROUND);
    topGround = createGround(TOP_GROUND);
}

H2DE_Object* Scenery::createGround(SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelPos& defaultBotGroundPos = game->getData()->getDefaultBotGroundPos();
    static const H2DE_LevelPos& defaultTopGroundPos = game->getData()->getDefaultTopGroundPos();
    static const H2DE_LevelSize& groundSize = game->getData()->getGroundSize();

    const H2DE_LevelPos pos = (type == BOT_GROUND) ? defaultBotGroundPos : defaultTopGroundPos;

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = game->convertToLevelPos(pos, groundSize).makeRect(groundSize);

    return H2DE_CreateBasicObject(engine, od, H2DE_BasicObjectData());
}

void Scenery::createLines() {
    lines.push_back(createLine(BOT_GROUND));
    lines.push_back(createLine(TOP_GROUND));
}

H2DE_Object* Scenery::createLine(SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();
    static const H2DE_LevelSize& lineSize = game->getData()->getLineSize();
    static const H2DE_ColorRGB& defaultLineColor = game->getData()->getDefaultLineColor();
    static const float& lineOffsetX = game->getData()->getLineOffsetX();
    const float camPosX = H2DE_GetCameraPos(engine).x;

    const H2DE_LevelPos pos = (type == BOT_GROUND)
        ? H2DE_LevelPos{ camPosX + lineOffsetX, displayedBotGroundPosY + groundTileSize.y - lineSize.y }
        : H2DE_LevelPos{ camPosX + lineOffsetX, displayedTopGroundPosY };

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = game->convertToLevelPos(pos, lineSize).makeRect(lineSize);
    od.index = getIndex(LAYER_G, 1);

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = "GJ_GameSheet02-uhd.png";
    sd.rect = H2DE_LevelPos{ 0.0f, 0.0f }.makeRect(lineSize);
    sd.scaleMode = H2DE_SCALE_MODE_LINEAR;
    sd.color = defaultLineColor;

    H2DE_TextureData td = H2DE_TextureData();
    td.srcRect = { 1, 13, 1803, 6 };

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = H2DE_CreateTexture(engine, sd, td);

    return H2DE_CreateBasicObject(engine, od, bod);
}

// CLEANUP
Scenery::~Scenery() {
    destroyGrounds();
    destroyTiles();
    destroyLines();
    stopTimelines();
}

void Scenery::destroyGrounds() {
    game->destroyObjects({ botGround, topGround });
}

void Scenery::destroyTiles() {
    game->destroyObjects(backgroundTiles);
    backgroundTiles.clear();

    game->destroyObjects(botGroundTiles);
    botGroundTiles.clear();

    game->destroyObjects(topGroundTiles);
    topGroundTiles.clear();
}

void Scenery::destroyLines() {
    game->destroyObjects(lines);
}

void Scenery::stopTimelines() {
    stopTimeline(backgroundColorTimlineID);
    stopTimeline(botGroundColorTimlineID);
    stopTimeline(topGroundColorTimlineID);
    stopTimeline(lineColorTimlineID);
}

void Scenery::stopTimeline(std::optional<unsigned int>& id) {
    static H2DE_Engine* engine = game->getEngine();

    if (id.has_value()) {
        H2DE_StopTimeline(engine, id.value(), false);
        id = std::nullopt;
    }
}

// UDPATE
void Scenery::update() {
    updatePositions();
    removeOutOfScreenTiles();
    addMissingTiles();
}

// positions
void Scenery::updatePositions() {
    updateBackgroundTilesPosition();
    updateGroundTilesPosition();
    updateGroundsPositions();
    updateLinesPosition();
}

void Scenery::updateBackgroundTilesPosition() {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelPos& defaultBackgroundPos = game->getData()->getDefaultBackgroundPos();
    static const H2DE_LevelSize& backgroundTileSize = game->getData()->getBackgroundTileSize();
    static const float backgroundRatio = game->getData()->getBackgroundRatio();

    const float minCamX = H2DE_GetCameraPos(engine).x;
    const float backgroundPosX = minCamX * backgroundRatio - 1.0f;

    float xOffset = backgroundPosX;
    while (xOffset + backgroundTileSize.x < minCamX) {
        xOffset += backgroundTileSize.x;
    }
    
    for (size_t i = 0; i < backgroundTiles.size(); i++) {
        H2DE_Object* tile = backgroundTiles[i];

        const float x = xOffset + (backgroundTileSize.x * i);
        const float y = displayedBackgroundPosY;

        H2DE_SetObjectPos(tile, game->convertToLevelPos({ x, y }, backgroundTileSize));
    }
}

void Scenery::updateGroundTilesPosition() {
    updateGroundTilesPosition(botGroundTiles, displayedBotGroundPosY);
    updateGroundTilesPosition(topGroundTiles, displayedTopGroundPosY);
}

void Scenery::updateGroundTilesPosition(const std::vector<H2DE_Object*>& tiles, float posY) {
    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();

    for (H2DE_Object* tile : tiles) {
        H2DE_LevelPos pos = { H2DE_GetObjectPos(tile).x, posY };
        H2DE_SetObjectPos(tile, game->convertToLevelPos(pos, groundTileSize));
    }
}

void Scenery::updateGroundsPositions() {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelSize& groundSize = game->getData()->getGroundSize();
    const float camPosX = H2DE_GetCameraPos(engine).x;

    const H2DE_LevelPos botPos = { camPosX, collisionBotGroundPosY }; 
    H2DE_SetObjectPos(botGround, game->convertToLevelPos(botPos, groundSize));

    const H2DE_LevelPos topPos = { camPosX, collisionTopGroundPosY }; 
    H2DE_SetObjectPos(topGround, game->convertToLevelPos(topPos, groundSize));
}

void Scenery::updateLinesPosition() {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();
    static const H2DE_LevelSize& lineSize = game->getData()->getLineSize();
    static const float& lineOffsetX = game->getData()->getLineOffsetX();
    const float camPosX = H2DE_GetCameraPos(engine).x;

    if (lines.size() != 2) {
        return;
    }

    const H2DE_LevelPos botPos = { camPosX + lineOffsetX, displayedBotGroundPosY + groundTileSize.y - lineSize.y }; 
    H2DE_SetObjectPos(lines[0], game->convertToLevelPos(botPos, lineSize));

    const H2DE_LevelPos topPos = { camPosX + lineOffsetX, displayedTopGroundPosY }; 
    H2DE_SetObjectPos(lines[1], game->convertToLevelPos(topPos, lineSize));
}

// remove tiles
void Scenery::removeOutOfScreenTiles() {
    static const H2DE_LevelSize& backgroundTileSize = game->getData()->getBackgroundTileSize();
    removeOutOfScreenTiles(backgroundTiles, backgroundTileSize);

    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();
    removeOutOfScreenTiles(botGroundTiles, groundTileSize);
    removeOutOfScreenTiles(topGroundTiles, groundTileSize);
}

void Scenery::removeOutOfScreenTiles(std::vector<H2DE_Object*>& tiles, const H2DE_LevelSize& tileSize) {
    static H2DE_Engine* engine = game->getEngine();
    const float minCamX = H2DE_GetCameraPos(engine).x;

    for (auto it = tiles.begin(); it != tiles.end(); ) {
        const H2DE_LevelRect tileRect = game->convertToLevelRect(game->convertToGdPos(H2DE_GetObjectPos(*it), tileSize).makeRect(tileSize));
    
        if (minCamX > tileRect.x + tileRect.w) {
            H2DE_DestroyObject(engine, *it);
            tiles.erase(it);
        }
        break;
    }
}

// add missing tiles
void Scenery::addMissingTiles() {
    static const H2DE_LevelPos& backgroundDefaultPos = game->getData()->getDefaultBackgroundPos();
    static const H2DE_LevelSize& backgroundTileSize = game->getData()->getBackgroundTileSize();
    addMissingTiles(backgroundTiles, backgroundDefaultPos, backgroundTileSize, BACKGROUND);

    static const H2DE_LevelPos& botGroundPos = game->getData()->getDefaultBotGroundPos();
    static const H2DE_LevelPos& topGroundPos = game->getData()->getDefaultTopGroundPos();
    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();
    addMissingTiles(botGroundTiles, botGroundPos, groundTileSize, BOT_GROUND);
    addMissingTiles(topGroundTiles, topGroundPos, groundTileSize, TOP_GROUND);
}

void Scenery::addMissingTiles(std::vector<H2DE_Object*>& tiles, const H2DE_LevelPos& defaultPos, const H2DE_LevelSize& tileSize, SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();
    const float camMaxX = H2DE_GetCameraPos(engine).x + H2DE_GetCameraSize(engine).x;

    float lastTilePosMaxX = !tiles.empty()
        ? H2DE_GetObjectPos(tiles.back()).x + tileSize.x
        : defaultPos.x;

    while (lastTilePosMaxX < camMaxX) {
        createTile(lastTilePosMaxX, type);
        lastTilePosMaxX += tileSize.x;
    }
}

// create tiles
void Scenery::createTile(float posX, SceneryType type) {
    switch (type) {
        case BACKGROUND: createBackgroundTile(posX); break;
        case BOT_GROUND: createGroundTile(posX, type); break;
        case TOP_GROUND: createGroundTile(posX, type); break;
        default: return;
    }
}

void Scenery::createBackgroundTile(float posX) {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelSize& backgroundTileSize = game->getData()->getBackgroundTileSize();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = game->convertToLevelPos(H2DE_LevelPos{ posX, displayedBackgroundPosY }, backgroundTileSize).makeRect(backgroundTileSize);
    od.index = getIndex(LAYER_BG, 0);

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = "game_bg_0" + std::to_string(backgroundID) + "_001-uhd.png";
    sd.rect = H2DE_LevelPos{ 0.0f, 0.0f }.makeRect(backgroundTileSize);
    sd.color = backgroundColor;

    H2DE_Surface* surface = H2DE_CreateTexture(engine, sd, H2DE_TextureData());
    
    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = surface;

    backgroundTiles.push_back(H2DE_CreateBasicObject(engine, od, bod));
}

void Scenery::createGroundTile(float posX, SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelSize& groundTileSize = game->getData()->getGroundTileSize();

    const H2DE_LevelPos pos = type == BOT_GROUND
        ? H2DE_LevelPos{ posX, displayedBotGroundPosY }
        : H2DE_LevelPos{ posX, displayedTopGroundPosY };

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = game->convertToLevelPos(pos, groundTileSize).makeRect(groundTileSize);
    od.index = getIndex(LAYER_G, 0);

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = "groundSquare_0" + std::to_string(groundID) + "_001-uhd.png";
    sd.rect = H2DE_LevelPos{ 0.0f, 0.0f }.makeRect(groundTileSize);
    sd.color = groundColor;

    if (type == TOP_GROUND) {
        sd.flip = H2DE_FLIP_Y;
    }

    H2DE_Surface* surface = H2DE_CreateTexture(engine, sd, H2DE_TextureData());
    
    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = surface;

    std::vector<H2DE_Object*>& tiles = type == BOT_GROUND ? botGroundTiles : topGroundTiles;
    tiles.push_back(H2DE_CreateBasicObject(engine, od, bod));
}

// ACTIONS
void Scenery::reset() {
    destroyTiles();
    stopTimelines();
}

// SETTER
void Scenery::setBackgroundColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend) {
    stopTimeline(backgroundColorTimlineID);
    backgroundColorTimlineID = setColors(backgroundTiles, backgroundColor, color, duration, defaultBlend);
}

void Scenery::setBackgroundColor(const H2DE_ColorRGB& color, float defaultBlend) {
    H2DE_ColorRGB blendedColor = H2DE_ColorRGB();
    blendedColor.r = static_cast<Uint8>(backgroundColor.r + (color.r - backgroundColor.r) * defaultBlend);
    blendedColor.g = static_cast<Uint8>(backgroundColor.g + (color.g - backgroundColor.g) * defaultBlend);
    blendedColor.b = static_cast<Uint8>(backgroundColor.b + (color.b - backgroundColor.b) * defaultBlend);
    blendedColor.a = static_cast<Uint8>(backgroundColor.a + (color.a - backgroundColor.a) * defaultBlend);

    setColors(backgroundTiles, blendedColor);
}

void Scenery::setGroundColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend) {
    stopTimeline(botGroundColorTimlineID);
    botGroundColorTimlineID = setColors(botGroundTiles, groundColor, color, duration, defaultBlend);

    stopTimeline(topGroundColorTimlineID);
    topGroundColorTimlineID = setColors(topGroundTiles, groundColor, color, duration, defaultBlend);
}   

void Scenery::setGroundColor(const H2DE_ColorRGB& color, float defaultBlend) {
    H2DE_ColorRGB blendedColor = H2DE_ColorRGB();
    blendedColor.r = static_cast<Uint8>(groundColor.r + (color.r - groundColor.r) * defaultBlend);
    blendedColor.g = static_cast<Uint8>(groundColor.g + (color.g - groundColor.g) * defaultBlend);
    blendedColor.b = static_cast<Uint8>(groundColor.b + (color.b - groundColor.b) * defaultBlend);
    blendedColor.a = static_cast<Uint8>(groundColor.a + (color.a - groundColor.a) * defaultBlend);

    setColors(botGroundTiles, blendedColor);
    setColors(topGroundTiles, blendedColor);
}  

void Scenery::setLineColor(const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend) {
    stopTimeline(lineColorTimlineID);
    lineColorTimlineID = setColors(lines, lineColor, color, duration, defaultBlend);
}

void Scenery::setLineColor(const H2DE_ColorRGB& color, float defaultBlend) {
    H2DE_ColorRGB blendedColor = H2DE_ColorRGB();
    blendedColor.r = static_cast<Uint8>(lineColor.r + (color.r - lineColor.r) * defaultBlend);
    blendedColor.g = static_cast<Uint8>(lineColor.g + (color.g - lineColor.g) * defaultBlend);
    blendedColor.b = static_cast<Uint8>(lineColor.b + (color.b - lineColor.b) * defaultBlend);
    blendedColor.a = static_cast<Uint8>(lineColor.a + (color.a - lineColor.a) * defaultBlend);

    setColors(lines, color);
}

void Scenery::setColors(const std::vector<H2DE_Object*>& tiles, const H2DE_ColorRGB& color) {
    for (H2DE_Object* tile : tiles) {
        H2DE_BasicObject* basicObjectTile = dynamic_cast<H2DE_BasicObject*>(tile);

        if (basicObjectTile) {
            H2DE_Surface* surface = H2DE_GetBasicObjectSurface(basicObjectTile, "main");
            H2DE_SetSurfaceColor(surface, color);
        }
    }
}

unsigned int Scenery::setColors(const std::vector<H2DE_Object*>& tiles, H2DE_ColorRGB& defaultColor, const H2DE_ColorRGB& color, unsigned int duration, float defaultBlend) {
    static H2DE_Engine* engine = game->getEngine();

    duration = std::round((1.0f - defaultBlend) * duration);

    H2DE_ColorRGB blendedDefaultColor = H2DE_ColorRGB();
    blendedDefaultColor.r = static_cast<Uint8>(defaultColor.r + (color.r - defaultColor.r) * defaultBlend);
    blendedDefaultColor.g = static_cast<Uint8>(defaultColor.g + (color.g - defaultColor.g) * defaultBlend);
    blendedDefaultColor.b = static_cast<Uint8>(defaultColor.b + (color.b - defaultColor.b) * defaultBlend);
    blendedDefaultColor.a = static_cast<Uint8>(defaultColor.a + (color.a - defaultColor.a) * defaultBlend);

    return H2DE_CreateTimeline(engine, duration, H2DE_EASING_LINEAR, [this, &tiles, blendedDefaultColor, &defaultColor, color](float blend) {
        H2DE_ColorRGB interpoledColor = H2DE_ColorRGB();
        interpoledColor.r = static_cast<Uint8>(blendedDefaultColor.r + (color.r - blendedDefaultColor.r) * blend);
        interpoledColor.g = static_cast<Uint8>(blendedDefaultColor.g + (color.g - blendedDefaultColor.g) * blend);
        interpoledColor.b = static_cast<Uint8>(blendedDefaultColor.b + (color.b - blendedDefaultColor.b) * blend);
        interpoledColor.a = static_cast<Uint8>(blendedDefaultColor.a + (color.a - blendedDefaultColor.a) * blend);

        defaultColor = interpoledColor;
        setColors(tiles, interpoledColor);

    }, nullptr, 0, true);
}
