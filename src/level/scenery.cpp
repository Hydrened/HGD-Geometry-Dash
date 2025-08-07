#include "level/scenery.h"

// INIT
Scenery::Scenery(Game* g, BackgroundID bID, GroundID gID) : game(g), backgroundID(bID), groundID(gID) {
    initDefaultValues();
    initBackground();
    initGrounds();

    update();
}

void Scenery::initDefaultValues() {
    static const Data* gameData = game->getData();

    static const H2DE_Translate& defaultBackgroundTranslate = gameData->getDefaultBackgroundTranslate();
    static const H2DE_Translate& defaultBotGroundTranslate = gameData->getDefaultBotGroundTranslate();
    static const H2DE_Translate& defaultTopGroundTranslate = gameData->getDefaultTopGroundTranslate();

    displayedBackgroundTranslateY = defaultBackgroundTranslate.y;
    collisionBotGroundTranslateY = defaultBotGroundTranslate.y;
    displayedBotGroundTranslateY = collisionBotGroundTranslateY;
    collisionTopGroundTranslateY = defaultTopGroundTranslate.y;
    displayedTopGroundTranslateY = collisionTopGroundTranslateY;

    static const H2DE_ColorRGB& defaultBackgroundColor = gameData->getDefaultBackgroundColor();
    static const H2DE_ColorRGB& defaultGroundColor = gameData->getDefaultGroundColor();
    static const H2DE_ColorRGB& defaultLineColor = gameData->getDefaultLineColor();

    backgroundColor = defaultBackgroundColor;
    groundColor = defaultGroundColor;
    lineColor = defaultLineColor;
}

void Scenery::initBackground() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();

    static const H2DE_Translate& defaultBackgroundTranslate = gameData->getDefaultBackgroundTranslate();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = defaultBackgroundTranslate;
    od.index = Data::getIndex(LAYER_BG, 0);

    displayedBackground = engine->createObject<H2DE_BasicObject>(od);
}

void Scenery::initGrounds() {
    static const Data* gameData = game->getData();

    static const H2DE_Translate& defaultBotGroundTranslate = gameData->getDefaultBotGroundTranslate();
    static const H2DE_Translate& defaultTopGroundTranslate = gameData->getDefaultTopGroundTranslate();

    std::array<std::tuple<H2DE_BasicObject*&, H2DE_BasicObject*&, H2DE_Translate, float>, 2> groundData = {
        std::tuple<H2DE_BasicObject*&, H2DE_BasicObject*&, H2DE_Translate, float>(displayedBotGround, botGroundHitbox, defaultBotGroundTranslate, 1.0f),
        std::tuple<H2DE_BasicObject*&, H2DE_BasicObject*&, H2DE_Translate, float>(displayedTopGround, topGroundHitbox, defaultTopGroundTranslate, -1.0f)
    };

    for (auto& [displayedObject, hitboxObject, translate, flipY] : groundData) {
        H2DE_ObjectData od = H2DE_ObjectData();
        od.transform.translate = translate;
        od.index = Data::getIndex(LAYER_G, 0);

        od.transform.scale.y *= flipY;

        displayedObject = createDisplayedGround(od);
        hitboxObject = createGroundHitbox(od);
    }
}

H2DE_BasicObject* Scenery::createDisplayedGround(const H2DE_ObjectData& objectData) const {
    static H2DE_Engine* engine = game->getEngine();
    const auto& [sd, td] = game->getData()->getSurfaceBuffer("line");

    H2DE_BasicObject* res = engine->createObject<H2DE_BasicObject>(objectData);
    res->addSurface<H2DE_Texture>("line", sd, td);

    return res;
}

H2DE_BasicObject* Scenery::createGroundHitbox(const H2DE_ObjectData& objectData) const {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();

    static const H2DE_ColorRGB groundHitboxColor = game->getData()->getHitboxColor("blue");
    static const H2DE_Scale& groundScale = gameData->getGroundScale();

    H2DE_BasicObject* res = engine->createObject<H2DE_BasicObject>(objectData);

    H2DE_Hitbox hitbox = H2DE_Hitbox();
    hitbox.transform.scale = groundScale;
    hitbox.color = groundHitboxColor;
    res->addHitbox("main", hitbox);

    return res;
}

// CLEANUP
Scenery::~Scenery() {
    destroyBackground();
    destroyGrounds();
    destroyTimelines();
}

void Scenery::destroyBackground() {
    static H2DE_Engine* engine = game->getEngine();
    engine->destroyObject(displayedBackground);
}

void Scenery::destroyGrounds() {
    static H2DE_Engine* engine = game->getEngine();

    const std::array<H2DE_BasicObject*, 4> groundsObjects = getGroundsObjects();
    for (H2DE_BasicObject* ground : groundsObjects) {
        engine->destroyObject(ground);
    }
}

void Scenery::destroyTimelines() {
    stopTimeline(backgroundColorTimeline);
    stopTimeline(botGroundColorTimeline);
    stopTimeline(topGroundColorTimeline);
    stopTimeline(lineColorTimeline);
}

// UPDATE
void Scenery::update() {
    updateTranslations();
    addMissingTiles();
    removeOutOfScreenTiles();
}

// -- positions
void Scenery::updateTranslations() {
    updateBackroundObjectTranslations();
    updateGroundObjectsTranslations();

    updateTilesTransitions();
}

void Scenery::updateBackroundObjectTranslations() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    const H2DE_Translate cameraTranslate = camera->getTranslate();

    H2DE_Translate backgroundTranslate = displayedBackground->getTranslate();
    backgroundTranslate.x = cameraTranslate.x;

    displayedBackground->setTranslate(backgroundTranslate);
}

void Scenery::updateGroundObjectsTranslations() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    const H2DE_Translate cameraTranslate = camera->getTranslate();

    const std::array<H2DE_BasicObject*, 4> groundsObjects = getGroundsObjects();

    for (H2DE_BasicObject* ground : groundsObjects) {
        H2DE_Translate groundTranslate = ground->getTranslate();
        groundTranslate.x = cameraTranslate.x;

        ground->setTranslate(groundTranslate);
    }
}

void Scenery::updateTilesTransitions() {
    static const Data* gameData = game->getData();
    static const H2DE_Scale& backgroundTileScale = gameData->getBackgroundTileScale();
    static const H2DE_Scale& groundTileScale = gameData->getGroundTileScale();

    updateTilesTransitions(displayedBackground, backgroundTileScale, SCENERY_TYPE_BACKGROUND);
    updateTilesTransitions(displayedBotGround, groundTileScale, SCENERY_TYPE_GROUND);
    updateTilesTransitions(displayedTopGround, groundTileScale, SCENERY_TYPE_GROUND);
}

void Scenery::updateTilesTransitions(H2DE_BasicObject* object, const H2DE_Scale& tileScale, SceneryType type) {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static float halfCameraWidth = camera->getGameScale().x * 0.5f;

    constexpr float PARALLAX_MULTIPLIER = 0.1f;

    H2DE_Translate cameraTranslate = camera->getTranslate();
    if (type == SCENERY_TYPE_BACKGROUND) {
        cameraTranslate.x *= PARALLAX_MULTIPLIER;
    }

    float offsetX = -std::fmod(cameraTranslate.x, tileScale.x); 

    std::vector<H2DE_Texture*> tiles = getTiles(object);
    for (int i = 0; i < tiles.size(); i++) {
        H2DE_Texture* tile = tiles.at(i);

        float x = -halfCameraWidth + offsetX + (i * tileScale.x);
        tile->setTranslate({ x, 0.0f });
    }
}

// -- add tiles
void Scenery::addMissingTiles() {
    static const H2DE_Scale& backgroundTileScale = game->getData()->getBackgroundTileScale();
    addMissingTiles(displayedBackground, backgroundTileScale, SCENERY_TYPE_BACKGROUND);

    static const H2DE_Scale& groundTileScale = game->getData()->getGroundTileScale();
    addMissingTiles(displayedBotGround, groundTileScale, SCENERY_TYPE_GROUND);
    addMissingTiles(displayedTopGround, groundTileScale, SCENERY_TYPE_GROUND);
}

void Scenery::addMissingTiles(H2DE_BasicObject* object, const H2DE_Scale& tileScale, SceneryType type) {
    static const Data* gameData = game->getData();
    static const H2DE_Scale& groundScale = gameData->getGroundScale();
    static const H2DE_Scale& backgroundScale = gameData->getBackgroundScale();

    const float lastTileTranslateX = getLastTileTranslateX(object, type);
    const float startX = lastTileTranslateX + tileScale.x;

    const H2DE_Scale sceneryTypeScale = (type == SCENERY_TYPE_BACKGROUND) ? backgroundScale : groundScale;
    const float maxSceneryTypeX = (H2DE::abs(sceneryTypeScale.x) * 0.5f) + (tileScale.x * 0.5f);

    for (float x = startX; x < maxSceneryTypeX; x += tileScale.x) {
        createTile(object, x, type);
    }
}

// -- create tile
void Scenery::createTile(H2DE_BasicObject* object, float translateX, SceneryType type) {
    static const Data* gameData = game->getData();

    bool isGround = (type == SCENERY_TYPE_GROUND);

    Data::SurfaceBuffer tileSurfaceBuffer = (isGround)
        ? gameData->getGroundTileSurfaceBuffer(groundID, translateX)
        : gameData->getBackgroundTileSurfaceBuffer(backgroundID, translateX);

    tileSurfaceBuffer.texture.color = (isGround) ? groundColor : backgroundColor;

    const int absoluteTranslateX = translateX + object->getTranslate().x;
    const std::string tileName = "tile_" + std::to_string(absoluteTranslateX);

    object->addSurface<H2DE_Texture>(tileName, tileSurfaceBuffer.surface, tileSurfaceBuffer.texture);
}

// -- remove tiles
void Scenery::removeOutOfScreenTiles() {
    removeOutOfScreenTiles(displayedBackground, SCENERY_TYPE_BACKGROUND);
    removeOutOfScreenTiles(displayedBotGround, SCENERY_TYPE_GROUND);
    removeOutOfScreenTiles(displayedTopGround, SCENERY_TYPE_GROUND);
}

void Scenery::removeOutOfScreenTiles(H2DE_BasicObject* object, SceneryType type) {
    static const Data* gameData = game->getData();
    static const H2DE_Scale& groundScale = gameData->getGroundScale();
    static const H2DE_Scale& backgroundScale = gameData->getBackgroundScale();

    const H2DE_Scale sceneryTypeScale = (type == SCENERY_TYPE_BACKGROUND) ? backgroundScale : groundScale;
    const float minX = (H2DE::abs(sceneryTypeScale.x) * -0.5f) - (H2DE::abs(sceneryTypeScale.y) * 0.5f);

    for (const auto& [name, surface] : object->getSurfaces()) {
        if (!isSurfaceTile(name)) {
            continue;
        }

        const float surfaceTranslateX = surface->getTranslate().x;

        if (surfaceTranslateX < minX) {
            object->removeSurface(name);
        }
    }
}

// ACTIONS
void Scenery::stopTimeline(H2DE_Timeline* timeline) {
    static H2DE_Engine* engine = game->getEngine();

    if (timeline != nullptr) {
        timeline->stop(false);
        timeline = nullptr;
    }
}

// GETTERS
std::vector<H2DE_Texture*> Scenery::getTiles(const H2DE_BasicObject* object) const {
    std::vector<H2DE_Texture*> res = {};

    for (const auto& [name, surface] : object->getSurfaces()) {

        if (!isSurfaceTile(name)) {
            continue;
        }

        const H2DE_Texture* texture = dynamic_cast<const H2DE_Texture*>(surface);
        if (texture == nullptr) {
            continue;
        }

        res.push_back(const_cast<H2DE_Texture*>(texture));
    }

    return res;
}

std::array<H2DE_Texture*, 2> Scenery::getLines() const {
    return {
        displayedBotGround->getSurface<H2DE_Texture>("line"),
        displayedTopGround->getSurface<H2DE_Texture>("line")
    };
}

float Scenery::getLastTileTranslateX(const H2DE_BasicObject* object, SceneryType type) const {
    static const Data* gameData = game->getData();
    static const H2DE_Scale& groundScale = gameData->getGroundScale();
    static const H2DE_Scale& backgroundScale = gameData->getBackgroundScale();

    const H2DE_Scale sceneryTypeScale = (type == SCENERY_TYPE_BACKGROUND) ? backgroundScale : groundScale;
    float res = H2DE::abs(sceneryTypeScale.x) * -0.5f - H2DE::abs(sceneryTypeScale.y) * 0.5f - 0.5f;

    for (H2DE_Texture* tile : getTiles(object)) {
        const float tileTranslateX = tile->getTranslate().x;

        if (res < tileTranslateX) {
            res = tileTranslateX;
        }
    }

    return res;
}

// SETTERS
void Scenery::setBackgroundColor(const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
    stopTimeline(backgroundColorTimeline);
    backgroundColorTimeline = setColor(displayedBackground, backgroundColor, color, duration, start, SCENERY_TYPE_BACKGROUND);
}

void Scenery::setGroundColor(const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
    stopTimeline(botGroundColorTimeline);
    botGroundColorTimeline = setColor(displayedBotGround, groundColor, color, duration, start, SCENERY_TYPE_GROUND);

    stopTimeline(topGroundColorTimeline);
    topGroundColorTimeline = setColor(displayedTopGround, groundColor, color, duration, start, SCENERY_TYPE_GROUND);
}

void Scenery::setLineColor(const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
    stopTimeline(botGroundColorTimeline);
    botGroundColorTimeline = setColor(displayedBotGround, lineColor, color, duration, start, SCENERY_TYPE_LINE);

    stopTimeline(topGroundColorTimeline);
    topGroundColorTimeline = setColor(displayedTopGround, lineColor, color, duration, start, SCENERY_TYPE_LINE);
}

void Scenery::setColor(const H2DE_BasicObject* object, const H2DE_ColorRGB& color, SceneryType type) {
    switch (type) {
        case SCENERY_TYPE_BACKGROUND: backgroundColor = color; break;
        case SCENERY_TYPE_GROUND: groundColor = color; break;
        case SCENERY_TYPE_LINE: lineColor = color; break;
        default: return;
    }

    if (type != SCENERY_TYPE_LINE) {
        for (H2DE_Texture* tile : getTiles(object)) {
            tile->setColor(color);
        }

    } else {
        for (H2DE_Texture* line : getLines()) {
            line->setColor(color);
        }
    }
}

H2DE_Timeline* Scenery::setColor(const H2DE_BasicObject* object, const H2DE_ColorRGB& defaultColor, const H2DE_ColorRGB& color, uint32_t duration, uint32_t start, SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();

    if (duration == 0) {
        setColor(object, color, type);
        return nullptr;
    }

    float defaultBlend = (start != 0) ? static_cast<float>(start) / static_cast<float>(duration) : 0.0f;

    H2DE_ColorRGB blendedDefaultColor = H2DE_ColorRGB();
    blendedDefaultColor.r = static_cast<uint8_t>(defaultColor.r + (color.r - defaultColor.r) * defaultBlend);
    blendedDefaultColor.g = static_cast<uint8_t>(defaultColor.g + (color.g - defaultColor.g) * defaultBlend);
    blendedDefaultColor.b = static_cast<uint8_t>(defaultColor.b + (color.b - defaultColor.b) * defaultBlend);
    blendedDefaultColor.a = static_cast<uint8_t>(defaultColor.a + (color.a - defaultColor.a) * defaultBlend);

    uint32_t durationFromStart = (duration > start) ? duration - start : 0;

    if (defaultBlend != 0.0f) {
        setColor(object, blendedDefaultColor, type);
        // std::cout << "blendedDefaultColor: " << blendedDefaultColor << std::endl;
    }

    // std::cout << "start: " << start << std::endl;
    // std::cout << "defaultBlend: " << defaultBlend << std::endl;
    // std::cout << "durationFromStart: " << durationFromStart << std::endl;

    return engine->createTimeline(durationFromStart, H2DE_EASING_LINEAR, [this, object, type, blendedDefaultColor, color](float blend) {
        H2DE_ColorRGB interpoledColor = H2DE_ColorRGB();
        interpoledColor.r = static_cast<uint8_t>(blendedDefaultColor.r + (color.r - blendedDefaultColor.r) * blend);
        interpoledColor.g = static_cast<uint8_t>(blendedDefaultColor.g + (color.g - blendedDefaultColor.g) * blend);
        interpoledColor.b = static_cast<uint8_t>(blendedDefaultColor.b + (color.b - blendedDefaultColor.b) * blend);
        interpoledColor.a = static_cast<uint8_t>(blendedDefaultColor.a + (color.a - blendedDefaultColor.a) * blend);
        // std::cout << "interpolated: " << interpoledColor << std::endl;
        // std::cout << "i";

        setColor(object, interpoledColor, type);
    }, nullptr, 0, true);
}
