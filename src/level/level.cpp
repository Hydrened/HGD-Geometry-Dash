#include "level/level.h"

#include "menu/transition.h"
#include "level/scenery.h"
#include "level/player.h"
#include "level/items/item.h"

// INIT
Level::Level(Game* g, Level_ID i) : game(g), id(i), checkpoint(Checkpoint()) {
    init();
}

Level::Level(Game* g, Level_ID i, const Checkpoint& c) : game(g), id(i), checkpoint(c) {
    init();
}

void Level::init() {
    Uint32 startTime = SDL_GetTicks();

    logLoadingTime([this]() {
        Transition* transition = new Transition(game, TRANSITION_STATE_IN, nullptr);

        initData();
        initCheckpoint();
        initCamera();
        initScenery();
        initItemBuffers();
        initItems();
        initPlayer();
        initStartingDelay();

        updateCamera();
        updateScenery();
    }, "level");
}

void Level::initData() {
    const std::string stringifiedID = std::to_string(id);
    data = H2DE_Json::read("data/levels/" + stringifiedID + ".gdd");
}

void Level::initCheckpoint() {
    static const Data* gameData = game->getData();
    constexpr H2DE_Translate levelOrigin = { 0.0f, 0.0f };

    if (checkpoint.translate != levelOrigin) {
        return;
    }

    checkpoint.speed = Data::getLevelSpeed(data);
    checkpoint.gamemode = Data::getLevelPlayerGamemode(data);
    checkpoint.size = Data::getLevelPlayerSize(data);
    checkpoint.gravity = Data::getLevelPlayerGravity(data);
}

void Level::initCamera() {
    static const Data* gameData = game->getData();
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static const H2DE_Translate& defaultCameraTranslate = gameData->getDefaultCameraTranslate();
    
    const H2DE_Translate cameraTranslate = defaultCameraTranslate.addX(checkpoint.translate.x); // temp => y missing
    camera->setTranslate(cameraTranslate);
}

void Level::initScenery() {
    static const Data* gameData = game->getData();

    uint8_t backgroundID = Data::getLevelBackgroundID(data);
    uint8_t groundID = Data::getLevelGroundID(data);

    scenery = new Scenery(game, backgroundID, groundID);
}

void Level::initItemBuffers() {
    initBlockBuffers();
    initTriggerBuffers();
}

void Level::initBlockBuffers() {
    blocksBuffer.reserve(data["blocks"].size());

    for (const json& block : data["blocks"]) {

        if (!block.contains("i")) {
            return throw std::runtime_error("Block has no ID");
        }

        if (!block.contains("p")) {
            return throw std::runtime_error("Block has no translate");
        }

        Level::BlockBuffer blockBuffer = Level::BlockBuffer();
        blockBuffer.itemData.id = H2DE_Json::getString(block["i"]);
        blockBuffer.itemData.translate = H2DE_Json::getVector2D<float>(block["p"]);

        if (block.contains("r")) {
            blockBuffer.blockData.rotation = H2DE_Json::getFloat(block["r"]);
        }

        if (block.contains("f")) {
            const std::string flip = block["f"];

            if (flip.find('x')) {
                blockBuffer.blockData.flip.x = -1.0f;
            }

            if (flip.find('y')) {
                blockBuffer.blockData.flip.y = -1.0f;
            }
        }

        blocksBuffer.push_back(blockBuffer);
    }

    std::sort(blocksBuffer.begin(), blocksBuffer.end(), [](const Level::BlockBuffer& a, const Level::BlockBuffer& b) {
        return (a.itemData.translate.x < b.itemData.translate.x);
    });
}

void Level::initTriggerBuffers() {
    triggersBuffer.reserve(data["triggers"].size());
    
    for (const json& trigger : data["triggers"]) {
        
        if (!trigger.contains("i")) {
            return throw std::runtime_error("Trigger has no ID");
        }

        if (!trigger.contains("p")) {
            return throw std::runtime_error("Trigger has no translate");
        }

        Level::TriggerBuffer triggerBuffer = Level::TriggerBuffer();
        triggerBuffer.itemData.id = H2DE_Json::getString(trigger["i"]);
        triggerBuffer.itemData.translate = H2DE_Json::getVector2D<float>(trigger["p"]);
        
        if (trigger.contains("d")) {
            triggerBuffer.triggerData.duration = static_cast<uint32_t>(H2DE_Json::getInteger(trigger["d"]));
        }

        if (trigger.contains("c")) {
            triggerBuffer.triggerData.color = H2DE_Json::getColorRGB(trigger["c"], false);
        }

        if (trigger.contains("tt")) {
            triggerBuffer.triggerData.touchTrigger = H2DE_Json::getBool(trigger["tt"]);
        }

        triggersBuffer.push_back(triggerBuffer);
    }

    std::sort(triggersBuffer.begin(), triggersBuffer.end(), [](const Level::TriggerBuffer& a, const Level::TriggerBuffer& b) {
        return (a.itemData.translate.x < b.itemData.translate.x);
    });
}

void Level::initItems() {
    updateBlocksBuffer();
    updateTriggersBuffer(); // temp => simulate trigger
}

void Level::initPlayer() {
    static Save* save = game->getSave();

    const PlayerIcons playerIcons = save->getPlayerIcons();
    player = new Player(game, this, scenery, checkpoint, playerIcons);
}

void Level::initStartingDelay() {
    static H2DE_Engine* engine = game->getEngine();
    static Save* save = game->getSave();
    static const Data* gameData = game->getData();

    uint32_t duration = gameData->getStartingLevelDelayDuration() + save->getTransitionDuration() * 0.5f;  

    startingDelayID = engine->delay(duration, [this]() {
        newAttempt();
        startingDelayID = H2DE_INVALID_DELAY_ID;
    }, true);
}

// CLEANUP
Level::~Level() {
    destroyItems();
    destroyScenery();
    destroyPlayer();
    destroyStartingDelay();
    destroyRespawningDelay();
    stopSong();
}

void Level::destroyItems() {
    for (Item* item : items) {
        delete item;
        item = nullptr;
    }

    items.clear();
    blocks.clear();
    triggers.clear();
}

void Level::destroyScenery() {
    delete scenery;
}

void Level::destroyPlayer() {
    delete player;
}

void Level::destroyStartingDelay() {
    static H2DE_Engine* engine = game->getEngine();

    if (startingDelayID == H2DE_INVALID_DELAY_ID) {
        engine->stopDelay(startingDelayID, false);
    }
}

void Level::destroyRespawningDelay() {
    static H2DE_Engine* engine = game->getEngine();

    if (respawningDelayID == H2DE_INVALID_DELAY_ID) {
        engine->stopDelay(respawningDelayID, false);
    }
}

void Level::stopSong() {
    static H2DE_Audio* audio = game->getEngine()->getAudio();
    audio->stopSong();
}

void Level::playCloseSfx() {
    static H2DE_Audio* audio = game->getEngine()->getAudio();
    audio->playSfx("close-level.ogg", 0, false);
}

// ACTIONS
void Level::close(const std::function<void()>& callback) {
    playCloseSfx();

    Transition* transition = new Transition(game, TRANSITION_STATE_OUT, [this, callback]() {
        game->getEngine()->resume();

        delete this;

        if (callback) {
            callback();
        }
    });
}

void Level::newAttempt() {
    static H2DE_Audio* audio = game->getEngine()->getAudio();

    initCamera();
    updateScenery();
    player->respawn();

    destroyItems();

    blockBufferIndex = 0;
    triggerBufferIndex = 0;

    updateBlocksBuffer();
    updateTriggersBuffer();

    audio->playSong(Data::getLevelSong(data), 0, true);

    attempts++;
    std::cout << "Attempt " << attempts << std::endl;
}

void Level::playerDied() {
    static H2DE_Engine* engine = game->getEngine();
    
    stopSong();

    respawningDelayID = engine->delay(1000, [this]() {
        newAttempt();
        respawningDelayID = H2DE_INVALID_DELAY_ID;
    }, true);
}

// UPDATE
void Level::update() {
    bool inStartingDelay = (startingDelayID != H2DE_INVALID_DELAY_ID);
    if (inStartingDelay) {
        return;
    }

    updateItemsBuffers();
    updateItemVector();
    
    updatePlayer();
    updateCamera();
    updateScenery();
}

// -- buffers
void Level::updateItemsBuffers() {
    updateBlocksBuffer();
    updateTriggersBuffer();
}

void Level::updateBlocksBuffer() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static const float& cameraItemPadding = game->getData()->getCameraItemPadding();

    const float maxCamX = camera->getWorldRect().getMaxX();

    for (int i = blockBufferIndex; i < blocksBuffer.size(); i++) {
        const Level::BlockBuffer& buffer = blocksBuffer[i];

        if (buffer.itemData.translate.x - cameraItemPadding > maxCamX) {
            blockBufferIndex = i;
            return;
        }

        Block* block = new Block(game, this, buffer.itemData, buffer.blockData);
        items.push_back(block);
        blocks.push_back(block);
    }
}

void Level::updateTriggersBuffer() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static const float& cameraItemPadding = game->getData()->getCameraItemPadding();

    const float maxCamX = camera->getWorldRect().getMaxX();

    for (int i = triggerBufferIndex; i < triggersBuffer.size(); i++) {
        const Level::TriggerBuffer& buffer = triggersBuffer[i];

        if (buffer.itemData.translate.x - cameraItemPadding > maxCamX) {
            triggerBufferIndex = i;
            return;
        }

        Trigger* trigger = new Trigger(game, this, buffer.itemData, buffer.triggerData);
        items.push_back(trigger);
        triggers.push_back(trigger);
    }
}

// -- items
void Level::updateItemVector() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();

    const H2DE_LevelRect camRect = camera->getWorldRect();
    const H2DE_Translate playerTranslate = player->getTranslate();

    for (auto it = items.begin(); it != items.end(); ) {
        Item* item = *it;

        if (item->hasToBeRemoved(camRect, playerTranslate)) {
            it = items.erase(it);
            destroyItem(item);

        } else {
            ++it;
        }
    }
}

void Level::destroyItem(Item* item) {
    Block* block = dynamic_cast<Block*>(item);
    Trigger* trigger = dynamic_cast<Trigger*>(item);

    if (block != nullptr) {
        auto blockIt = std::find(blocks.begin(), blocks.end(), block);
        if (blockIt != blocks.end()) {
            blocks.erase(blockIt);
        }

    } else if (trigger != nullptr) {
        auto triggerIt = std::find(triggers.begin(), triggers.end(), trigger);
        if (triggerIt != triggers.end()) {
            triggers.erase(triggerIt);
        }
    }

    delete item;
}

// -- player
void Level::updatePlayer() {
    if (!player->isDead()) {
        player->update();
    }
}

// -- camera
void Level::updateCamera() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static const Data* gameData = game->getData();

    static const float& camearaOffsetXFromPlayer = gameData->getCamearaOffsetXFromPlayer();

    float x = player->getTranslate().x + camearaOffsetXFromPlayer;
    float y = camera->getTranslate().y;

    camera->setTranslate({ x, y });
}

// -- scenery
void Level::updateScenery() {
    scenery->update();
}
