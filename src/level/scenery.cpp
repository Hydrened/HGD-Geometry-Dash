#include "level/scenery.h"

// INIT
Scenery::Scenery(Game* g, Speed s, uint8_t bID, uint8_t gID) : game(g), speed(s), backgroundID(bID), groundID(gID) {
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

    updateBackroundTilesTranslations();
    updateGroundTilesTranslations();
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

void Scenery::updateBackroundTilesTranslations() {
    static const Data* gameData = game->getData();
    constexpr float PARALLAX_MULTIPLIER = 0.1f;

    const float& speedVelocityX = gameData->getSpeedVelocityX(speed);
    const float parallaxSpeedValocityX = speedVelocityX * PARALLAX_MULTIPLIER;

    for (const auto& [name, surface] : displayedBackground->getSurfaces()) {
        H2DE_Translate surfaceTranslate = surface->getTranslate();
        surfaceTranslate.x -= parallaxSpeedValocityX;

        surface->setTranslate(surfaceTranslate);
    }
}

void Scenery::updateGroundTilesTranslations() {
    static const Data* gameData = game->getData();
    
    const float& speedVelocityX = gameData->getSpeedVelocityX(speed);
    const std::array<H2DE_BasicObject*, 2> groundsDisplayedObjects = getGroundDisplayedObjects();

    for (H2DE_BasicObject* ground : groundsDisplayedObjects) {
        for (H2DE_Texture* tile : getTiles(ground)) {

            H2DE_Translate tileTranslate = tile->getTranslate();
            tileTranslate.x -= speedVelocityX;

            tile->setTranslate(tileTranslate);
        }
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
void Scenery::stopTimeline(H2DE_TimelineID& id) {
    static H2DE_Engine* engine = game->getEngine();

    if (id != H2DE_INVALID_TIMELINE_ID) {
        engine->stopTimeline(id, false);
        id = H2DE_INVALID_TIMELINE_ID;
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
    stopTimeline(backgroundColorTimelineID);
    backgroundColorTimelineID = setColor(displayedBackground, backgroundColor, color, duration, start, SCENERY_TYPE_BACKGROUND);
}

void Scenery::setGroundColor(const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
    stopTimeline(botGroundColorTimelineID);
    botGroundColorTimelineID = setColor(displayedBotGround, groundColor, color, duration, start, SCENERY_TYPE_GROUND);

    stopTimeline(topGroundColorTimelineID);
    topGroundColorTimelineID = setColor(displayedTopGround, groundColor, color, duration, start, SCENERY_TYPE_GROUND);
}

void Scenery::setLineColor(const H2DE_ColorRGB& color, uint32_t duration, uint32_t start) {
    stopTimeline(botGroundColorTimelineID);
    botGroundColorTimelineID = setColor(displayedBotGround, lineColor, color, duration, start, SCENERY_TYPE_LINE);

    stopTimeline(topGroundColorTimelineID);
    topGroundColorTimelineID = setColor(displayedTopGround, lineColor, color, duration, start, SCENERY_TYPE_LINE);
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

H2DE_TimelineID Scenery::setColor(const H2DE_BasicObject* object, const H2DE_ColorRGB& defautlColor, const H2DE_ColorRGB& color, uint32_t duration, uint32_t start, SceneryType type) {
    static H2DE_Engine* engine = game->getEngine();

    if (duration == 0) {
        setColor(object, color, type);
        return H2DE_INVALID_TIMELINE_ID;
    }

    uint32_t durationFromStart = (duration > start) ? duration - start : 0;

    return engine->createTimeline(durationFromStart, H2DE_EASING_LINEAR, [](float blend) {
        // todo
    }, nullptr, 0, true);
}
