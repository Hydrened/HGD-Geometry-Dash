#include "level/level.h"

#include "menu/menu_transition.h"
#include "level/scenery.h"
#include "level/player.h"

// INIT
Level::Level(Game* g, Level_ID i) : game(g), id(i), checkpoint(Checkpoint()) {
    init();
}

Level::Level(Game* g, Level_ID i, const Checkpoint& c) : game(g), id(i), checkpoint(c) {
    init();
}

void Level::init() {
    Uint32 startTime = SDL_GetTicks();
    
    MenuTransition* transition = new MenuTransition(game, TRANSITION_STATE_IN, nullptr);

    initData();
    initCheckpoint();
    initScenery();
    initCamera();
    initItemBuffers();
    initPlayer();

    Uint32 elapsedTime = SDL_GetTicks() - startTime;
    std::cout << "Opened level " << static_cast<int>(id) << " in " << elapsedTime << "ms" << std::endl;

    newAttempt();
}

void Level::initData() {
    const std::string stringifiedID = std::to_string(id);
    data = H2DE_Json::read("data/levels/" + stringifiedID + ".gdd");
}

void Level::initCheckpoint() {
    static const Data* gameData = game->getData();
    constexpr H2DE_Translate levelOrigin = { 0.0f, 0.0f };

    if (checkpoint.pos != levelOrigin) {
        return;
    }

    checkpoint.speed = Data::getLevelSpeed(data);
    checkpoint.gamemode = Data::getLevelPlayerGamemode(data);
    checkpoint.size = Data::getLevelPlayerSize(data);
    checkpoint.gravity = Data::getLevelPlayerGravity(data);
}

void Level::initScenery() {
    static const Data* gameData = game->getData();

    uint8_t backgroundID = Data::getLevelBackgroundID(data);
    uint8_t groundID = Data::getLevelGroundID(data);

    scenery = new Scenery(game, checkpoint.speed, backgroundID, groundID);
}

void Level::initCamera() {
    static const Data* gameData = game->getData();
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    
    camera->setTranslate(gameData->getDefaultCameraTranslate());
}

void Level::initItemBuffers() {
    initBlockBuffers();
    initTriggerBuffers();
}

void Level::initBlockBuffers() {
    for (const json& block : data["blocks"]) {

        if (!block.contains("i")) {
            return throw std::runtime_error("Block has no ID");
        }

        if (!block.contains("p")) {
            return throw std::runtime_error("Block has no translate");
        }

        BlockBuffer blockBuffer = BlockBuffer();
        blockBuffer.id = H2DE_Json::getString(block["i"]);
        blockBuffer.translate = H2DE_Json::getVector2D<float>(block["p"]);

        if (block.contains("r")) {
            blockBuffer.rotation = H2DE_Json::getFloat(block["r"]);
        }

        if (block.contains("f")) {
            blockBuffer.flip = static_cast<BlockFlip>(H2DE_Json::getInteger(block["f"]));
        }

        blockBuffers.push_back(blockBuffer);
    }
}

void Level::initTriggerBuffers() {
    for (const json& trigger : data["triggers"]) {
        
        if (!trigger.contains("i")) {
            return throw std::runtime_error("Trigger has no ID");
        }

        if (!trigger.contains("p")) {
            return throw std::runtime_error("Trigger has no translate");
        }

        TriggerBuffer triggerBuffer = TriggerBuffer();
        triggerBuffer.id = H2DE_Json::getString(trigger["i"]);
        triggerBuffer.translate = H2DE_Json::getVector2D<float>(trigger["p"]);
        
        if (trigger.contains("d")) {
            triggerBuffer.duration = static_cast<uint32_t>(H2DE_Json::getInteger(trigger["d"]));
        }

        if (trigger.contains("c")) {
            triggerBuffer.color = H2DE_Json::getColorRGB(trigger["c"], false);
        }

        if (trigger.contains("tt")) {
            triggerBuffer.touchTrigger = H2DE_Json::getBool(trigger["tt"]);
        }

        triggerBuffers.push_back(triggerBuffer);
    }
}

void Level::initPlayer() {
    static Save* save = game->getSave();

    const PlayerIcons playerIcons = save->getPlayerIcons();
    player = new Player(game, scenery, checkpoint, playerIcons);
}

// CLEANUP
Level::~Level() {
    destroyScenery();
    destroyPlayer();
}

void Level::destroyScenery() {
    delete scenery;
}

void Level::destroyPlayer() {
    delete player;
}

// ACTIONS
void Level::close(const std::function<void()>& callback) {
    MenuTransition* transition = new MenuTransition(game, TRANSITION_STATE_OUT, [this, callback]() {
        game->getEngine()->resume();

        delete this;

        if (callback) {
            callback();
        }
    });
}

void Level::newAttempt() {
    attempts++;
    std::cout << "Attempt " << attempts << std::endl;
}

// UPDATE
void Level::update() {
    static const Data* gameData = game->getData();
    static H2DE_Camera* camera = game->getEngine()->getCamera();

    const float& speed = gameData->getSpeedVelocityX(player->getSpeed());

    H2DE_Translate cameraTranslate = camera->getTranslate();
    cameraTranslate.x += speed;

    camera->setTranslate(cameraTranslate);
    scenery->update();
    player->update();

    // update triggers
}
