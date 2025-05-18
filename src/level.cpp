#include "level.h"

// INIT
Level::Level(Game* g, int i) : game(g), id(i), checkpoint(game->getData()->getDefaultCheckpoint()) {
    init();
}

Level::Level(Game* g, int i, const Checkpoint& c) : game(g), id(i), checkpoint(c) {
    init();
}

void Level::init() {
    Uint32 startTime = SDL_GetTicks();

    loadData();
    initItemBuffer();
    initCamera();
    initScenery();
    initPlayer();
    initItems();
    initSpeed();
    initStartDelay();

    Uint32 elapsedTime = SDL_GetTicks() - startTime;
    std::cout << "Opened level in " << elapsedTime << "ms" << std::endl;
}

void Level::loadData() {
    data = H2DE_ReadJsonFile("data/levels/" + std::to_string(id) + ".gdd");
}

void Level::initItemBuffer() {
    const int nbBlocks = data["blocks"].size();
    const int nbTriggers = data["triggers"].size();
    itemBuffer.reserve(nbBlocks + nbTriggers);

    initItemBlockBuffer();
    initItemTriggerBuffer();

    std::sort(itemBuffer.begin(), itemBuffer.end(), [](const ItemData& a, const ItemData& b) {
        return (a.minX < b.minX);
    });
}

void Level::initItemBlockBuffer() {
    static const Data* gameData = game->getData();

    for (const json& block : data["blocks"]) {
        ItemData itemData = getItemData(block);
        itemData.blockData = gameData->getBlockData(itemData.id, block);
        itemData.initMaximums();
        
        itemBuffer.push_back(itemData);
    }
}

void Level::initItemTriggerBuffer() {
    static const Data* gameData = game->getData();

    for (const json& trigger : data["triggers"]) {
        ItemData itemData = getItemData(trigger);
        itemData.triggerData = gameData->getTriggerData(itemData.id, trigger);
        itemData.initMaximums();

        itemBuffer.push_back(itemData);
    }
}

void Level::initCamera() const {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelPos& defaultCameraPos = game->getData()->getDefaultCameraPos();
    static const H2DE_LevelPos& startposCameraOffset = game->getData()->getStartposCameraOffset();
    static const float& minCamY = game->getData()->getMinCamY();
    static const float& maxCamY = game->getData()->getMaxCamY();

    H2DE_LevelPos camPos = defaultCameraPos;

    if (checkpoint.camPosY.has_value()) {
        camPos.x = checkpoint.pos.x + startposCameraOffset.x;
        camPos.y = checkpoint.camPosY.value();

    } else {
        camPos = checkpoint.pos + startposCameraOffset;
    }


    camPos.y = std::clamp(camPos.y, minCamY, maxCamY);
    H2DE_SetCameraPos(engine, game->convertToCamPos(camPos));
}

void Level::initScenery() {
    json config = data["config"];
    json background = config["background"];
    json ground = config["ground"];

    const int backgroundID = static_cast<int>(background["id"]);
    const int groundID = static_cast<int>(ground["id"]);

    scenery = new Scenery(game, backgroundID, groundID);
}

void Level::initPlayer() {
    player = new Player(game, this, checkpoint);
}

void Level::initItems() {
    updateItemBuffer();
    simulateTriggers(player->getPos().x);
    updateItems();
}

void Level::initSpeed() {
    static const Data* gameData = game->getData();

    int s = H2DE_GetIntFromJson(data["config"]["speed"]);

    bool isCheckpointDefault = (checkpoint.pos.x == 0.0f);
    if (!isCheckpointDefault) {
        s = checkpoint.speed;
    }

    s = std::clamp(s, 0, 4);
    setSpeed(s);
}

void Level::initStartDelay() {
    static H2DE_Engine* engine = game->getEngine();
    static const Save* save = game->getSave();

    const int transitionDuration = save->getTransitionDuration();

    startDelayID = H2DE_Delay(engine, 1000 + transitionDuration, [this]() {
        startDelayID = -1;
        playSong();
    }, true);
}

void Level::playSong() const {
    static H2DE_Engine* engine = game->getEngine();

    const std::string songName = data["config"]["song"];
    H2DE_PlaySong(engine, songName, 0, true);
}

// CLEANUP
Level::~Level() {
    stopStartDelay();
    stopRespawnDelay();
    destroyScenery();
    destroyPlayer();
    destroyItems();
    isValid = false;
}

void Level::stopStartDelay() {
    static H2DE_Engine* engine = game->getEngine();
    
    if (startDelayID != -1) {
        H2DE_StopDelay(engine, startDelayID, false);
        startDelayID = -1;
    }
}

void Level::stopRespawnDelay() {
    static H2DE_Engine* engine = game->getEngine();

    if (respawnDelayID != -1) {
        H2DE_StopDelay(engine, respawnDelayID, false);
        respawnDelayID = -1;
    }
}

void Level::destroyScenery() {
    delete scenery;
}

void Level::destroyPlayer() {
    delete player;
}

void Level::destroyItems() {
    for (Item* item : items) {
        if (item) {
            delete item;
            item = nullptr;
        }
    }

    items.clear();
}

// UPDATE
void Level::update() {
    if (startDelayID != -1) {
        return;
    }

    updatePlayer();
    updateCamera();
    updateScenery();
    updateItemBuffer();
    updateItems();
}

void Level::updateCamera() {
    static H2DE_Engine* engine = game->getEngine();
    static const float cameraHeight = H2DE_GetCameraSize(engine).y;
    static const float& cameraOffsetX = game->getData()->getStartposCameraOffset().x;
    static const float& cameraBlockPadding = game->getData()->getCameraBlockPadding();

    H2DE_LevelPos initialCamPos = H2DE_GetCameraPos(engine);
    const H2DE_LevelPos playerPos = player->getObjectPos();
    const H2DE_LevelVelocity playerVelocity = player->getVelocity();

    const float cameraTopPaddingY = initialCamPos.y + cameraBlockPadding;
    const float cameraBottomPaddingY = initialCamPos.y + cameraHeight - cameraBlockPadding;

    bool isPlayerInTopCameraPadding = (cameraTopPaddingY > playerPos.y);
    bool isPlayerInBottomCameraPadding = (cameraBottomPaddingY < playerPos.y);

    // if (isPlayerInTopCameraPadding && playerVelocity.y <= 0.0f) {
    //     const float blend = std::clamp((cameraTopPaddingY - playerPos.y) / cameraBlockPadding, 0.0f, 1.0f);
    //     initialCamPos.y += playerVelocity.y * blend;
    // }

    // if (isPlayerInBottomCameraPadding && playerVelocity.y >= 0.0f) {
    //     const float blend = std::clamp((cameraBottomPaddingY + playerPos.y) / cameraBlockPadding, 0.0f, 1.0f);
    //     initialCamPos.y -= playerVelocity.y * blend;
    // }

    const float finalX = playerPos.x + cameraOffsetX;
    const float finalY = initialCamPos.y;

    H2DE_SetCameraPos(engine, { finalX, finalY });
}

void Level::updateScenery() {
    scenery->update();
}

void Level::updatePlayer() {
    bool playerIsDead = (respawnDelayID != -1);

    if (!playerIsDead) {
        player->update();
    }
}

void Level::updateItemBuffer() {
    static H2DE_Engine* engine = game->getEngine();
    static H2DE_LevelSize camSize = H2DE_GetCameraSize(engine);

    const H2DE_LevelPos camPos = H2DE_GetCameraPos(engine);
    const float maxCamX = camPos.x + camSize.x;

    ItemData buffer = itemBuffer[itemBufferIndex];

    while (buffer.minX < maxCamX) {
        if (itemBufferIndex >= itemBuffer.size()) {
            break;
        }

        buffer = itemBuffer[itemBufferIndex];

        bool isBlock = buffer.blockData.has_value();
        bool isTrigger = buffer.triggerData.has_value();

        if (isBlock) {
            Block* block = new Block(game, buffer, buffer.blockData.value());
            items.push_back(block);

        } else if (isTrigger) {
            Trigger* trigger = new Trigger(game, buffer, buffer.triggerData.value());
            items.push_back(trigger);

        } else {
            break;
        }

        itemBufferIndex++;
    }
}

void Level::updateItems() {
    static H2DE_Engine* engine = game->getEngine();
    static H2DE_LevelSize camSize = H2DE_GetCameraSize(engine);

    const H2DE_LevelPos camPos = H2DE_GetCameraPos(engine);
    const H2DE_LevelPos playerPos = player->getPos();

    for (auto it = items.begin(); it != items.end(); ) {
        Item* item = *it;

        if (item->hasToBeRemoved(camPos, playerPos)) {
            it = items.erase(it);
            delete item;

        } else {
            item->update();
            ++it;
        }
    }
}

// ACTIONS
void Level::respawn() {
    reset();

    player->respawn(checkpoint);
    updateItemBuffer();
    simulateTriggers(player->getPos().x);
}

void Level::reset() {
    respawnDelayID = -1;
    itemBufferIndex = 0;

    destroyItems();
    resetScenery();
}

void Level::resetScenery() {
    json config = data["config"];
    json background = config["background"];
    json ground = config["ground"];
    json line = config["line"];

    const H2DE_ColorRGB backgroundColor = H2DE_GetColorRgbFromJson(background["color"], false);
    const H2DE_ColorRGB groundColor = H2DE_GetColorRgbFromJson(ground["color"], false);
    const H2DE_ColorRGB lineColor = H2DE_GetColorRgbFromJson(line["color"], false);

    scenery->reset();

    scenery->setBackgroundColor(backgroundColor);
    scenery->setGroundColor(groundColor);
    scenery->setLineColor(lineColor);
}

void Level::simulateTriggers(float posX) {
    const int playerPosTime = getPosTime(posX);
    std::vector<std::pair<const Trigger*, float>> triggers = {};

    for (const Item* item : items) {

        const Trigger* trigger = dynamic_cast<const Trigger*>(item);
        if (!trigger) {
            continue;
        }

        const float triggerPosX = trigger->getPos().x;
        if (triggerPosX > posX) {
            break;
        }

        const float blend = std::clamp((playerPosTime - getPosTime(triggerPosX)) / static_cast<float>(trigger->getDuration()), 0.0f, 1.0f);
        triggers.push_back({ trigger, blend });
    }

    for (const auto& [trigger, blend] : triggers) {
        trigger->activate(this, blend);
    }
}

void Level::removeTrigger(const Trigger* trigger) {
    auto it = std::remove(items.begin(), items.end(), trigger);
    
    if (it != items.end()) {
        items.erase(it, items.end());
    }
}

// GETTER
const ItemData Level::getItemData(const json& item) {
    ItemData ib = ItemData();
    ib.id = item["i"];
    ib.pos = H2DE_GetPosFromJson<float>(item["p"]);

    return ib;
}

const H2DE_Object* Level::getBotGround() const {
    return scenery->getBotGround();
}

const H2DE_Object* Level::getTopGround() const {
    return scenery->getTopGround();
}

const int Level::getPosTime(float posX) const {
    static const Data* gameData = game->getData();
    static const unsigned int fps = H2DE_GetFps(game->getEngine());

    std::vector<std::pair<float, int>> speeds = {};

    for (const Item* item : items) {
        const Block* block = dynamic_cast<const Block*>(item);
        if (!block) {
            continue;
        }

        const float blockPosX = block->getPos().x;
        if (blockPosX > posX + 1.0f) {
            break;
        }

        const std::optional<int> possibleSpeedValue = block->getSpeedValue();
        if (possibleSpeedValue.has_value()) {
            speeds.push_back({ blockPosX, possibleSpeedValue.value() });
        }
    }

    const int defaultSpeed = H2DE_GetIntFromJson(data["config"]["speed"]);

    int currentSpeed = defaultSpeed;
    float currentPosX = 0.0f;
    int nbFrames = 0;

    while (currentPosX < posX) {
        currentPosX += gameData->getSpeed(currentSpeed);

        if (speeds.size() > 0) {
            if (currentPosX >= speeds[0].first) {
                currentSpeed = speeds[0].second;
                speeds.erase(speeds.begin());
            }
        }

        nbFrames++;
    }

    return std::round(nbFrames / static_cast<float>(fps) * 1000.0f);
}

// SETTER
void Level::setSpeed(int s) {
    speed = s;
    player->setVelocityX(game->getData()->getSpeed(speed));
}

void Level::setBackgroundColor(const H2DE_ColorRGB& color, int duration, float blend) {
    scenery->setBackgroundColor(color, duration, blend);
}

void Level::setGroundColor(const H2DE_ColorRGB& color, int duration, float blend) {
    scenery->setGroundColor(color, duration, blend);
}

void Level::setLineColor(const H2DE_ColorRGB& color, int duration, float blend) {
    scenery->setLineColor(color, duration, blend);
}

void Level::playerDied() {
    static H2DE_Engine* engine = game->getEngine();

    respawnDelayID = H2DE_Delay(engine, 1000, [this]() {
        respawn();
    }, true);
}
