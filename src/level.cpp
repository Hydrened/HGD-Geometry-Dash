#include "level.h"

// INIT
Level::Level(Game* g, int i, const Checkpoint* c) : game(g), id(i), checkpoint(c) {
    Uint32 startTime = SDL_GetTicks();

    loadData();
    initItemBuffer();
    initCamera();
    initScenery();
    initPlayer();
    initSpeed();
    initStartDelay();

    Uint32 elapsedTime = SDL_GetTicks() - startTime;
    std::cout << "Opened level in " << elapsedTime << "ms" << std::endl;
}

void Level::loadData() {
    data = H2DE_ReadJsonFile("data/levels/" + std::to_string(id) + ".gdd");
}

void Level::initItemBuffer() {
    initItemBlockBuffer();
    initItemTriggerkBuffer();

    std::sort(itemBuffer.begin(), itemBuffer.end(), [](const auto& a, const auto& b) {
        return a.first.pos.x < b.first.pos.x;
    });
}

void Level::initItemBlockBuffer() {
    for (const json& block : data["blocks"]) {
        BlockBuffer bb = BlockBuffer();
        bb.rotation = block.contains("r")
            ? static_cast<float>(block["r"])
            : 0.0f;
        bb.flip = block.contains("f")
            ? static_cast<H2DE_Flip>(block["f"])
            : H2DE_FLIP_NONE;

        itemBuffer.push_back({ Level::getItemBuffer(block), { bb, std::nullopt }});
    }
}

void Level::initItemTriggerkBuffer() {
    for (const json& trigger : data["triggers"]) {
        TriggerBuffer td = TriggerBuffer();
        td.duration = 0;

        itemBuffer.push_back({ Level::getItemBuffer(trigger), { std::nullopt, td }});
    }
}

void Level::initCamera() const {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelPos& defaultCameraPos = game->getData()->getDefaultCameraPos();

    const H2DE_LevelPos camPos = (checkpoint == nullptr)
        ? defaultCameraPos
        : checkpoint->camPos;

    H2DE_SetCameraPos(engine, game->convertToCamPos(camPos));
}

void Level::initScenery() {
    json config = data["config"];
    json background = config["background"];
    json ground = config["ground"];
    json line = config["line"];

    const int backgroundID = static_cast<int>(background["id"]);
    const int groundID = static_cast<int>(ground["id"]);

    const H2DE_ColorRGB backgroundColor = getColorFromJson(background["color"]);
    const H2DE_ColorRGB groundColor = getColorFromJson(ground["color"]);
    const H2DE_ColorRGB lineColor = getColorFromJson(line["color"]);

    scenery = new Scenery(game, backgroundID, groundID);

    scenery->setBackgroundColor(backgroundColor);
    scenery->setGroundColor(groundColor);
    scenery->setLineColor(lineColor);
}

void Level::initPlayer() {
    player = new Player(game, checkpoint);
}

void Level::initSpeed() {
    static const Data* gameData = game->getData();

    const int s = (checkpoint == nullptr)
        ? static_cast<int>(data["config"]["speed"])
        : gameData->getSpeed(checkpoint->velocity.x);

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
    destroyScenery();
    destroyPlayer();
    destroyItems();
}

void Level::stopStartDelay() {
    static H2DE_Engine* engine = game->getEngine();
    
    if (startDelayID != -1) {
        H2DE_StopDelay(engine, startDelayID, false);
        startDelayID = -1;
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

    updateCamera();
    updateScenery();
    updatePlayer();
    updateItemBuffer();
    updateItems();
}

void Level::updateCamera() {
    game->updateCamera(speed);
}

void Level::updateScenery() {
    scenery->update();
}

void Level::updatePlayer() {
    player->update();
}

void Level::updateItemBuffer() {
    static H2DE_Engine* engine = game->getEngine();
    static H2DE_LevelSize camSize = H2DE_GetCameraSize(engine);

    const H2DE_LevelPos camPos = H2DE_GetCameraPos(engine);
    const float maxCamX = camPos.x + camSize.x;

    while (itemBuffer[itemBufferIndex].first.pos.x < maxCamX) {
        const auto& buffer = itemBuffer[itemBufferIndex];

        bool isBlock = buffer.second.first.has_value();
        bool isTrigger = buffer.second.second.has_value();

        if (isBlock) {
            Block* block = new Block(game, buffer.first, buffer.second.first.value());
            items.push_back(block);

        } else if (isTrigger) {
            Trigger* trigger = new Trigger(game, buffer.first, buffer.second.second.value());
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

// GETTER
const ItemBuffer Level::getItemBuffer(const json& item) {
    ItemBuffer ib = ItemBuffer();
    ib.id = item["i"];
    ib.pos = getPosFromJson(item["p"]);

    return ib;
}

Player* Level::getPlayer() const {
    return player;
}

const int Level::getSpeed() const {
    return speed;
}

// SETTER
void Level::setSpeed(int s) {
    speed = s;
    player->setVelocityX(game->getData()->getSpeed(speed));
}
