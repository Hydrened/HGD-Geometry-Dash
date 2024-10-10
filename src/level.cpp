#include "level.h"

// INIT
Level::Level(Game* g, int i) : game(g), id(i) {
    static H2DE_Engine* engine = game->getEngine(); 

    std::cout << "Initializing level..." << std::endl;
    Uint32 start = SDL_GetTicks();
    data = LevelLoader::getLevelData(id);

    initFromSave();
    initCamera();
    initLevelElements();
    initItems();
    initConfig();

    player = new Player(game, this, &data->startpos);
    updateBackgroundY();
    
    std::cout << "Level initialized in " << SDL_GetTicks() - start << "ms" << std::endl;
    std::cout << "Attempt " << attempts << std::endl;

    Game::delay(1000, [this]() {
        game->setState({ LEVEL_PLAYING, DEFAULT });
        H2DE_PlaySong(engine, data->song, 0);
    }); // error => rare crash
}

void Level::initFromSave() {
    json* saves = H2DE_Json::read("data/saves.json");
    std::string stingifiedID = std::to_string(id);

    if ((*saves)["levels"].contains(stingifiedID)) {
        attempts = static_cast<int>((*saves)["levels"][stingifiedID]["attempts"]) + 1;
        for (bool c : (*saves)["levels"][stingifiedID]["coins"]) coins.push_back(c);
        bestNormalMode = (*saves)["levels"][stingifiedID]["progress"]["normal"];
        bestPracticeMode = (*saves)["levels"][stingifiedID]["progress"]["practice"];

    } else {
        attempts = 1;
        coins = { false, false, false };
        bestNormalMode = 0.0f;
        bestPracticeMode = 0.0f;
    }
}

void Level::initCamera() {
    game->getCamera()->globalSet(data->startpos.camPos);
}

void Level::initLevelElements() {
    backgroundPos = gameData->positions->background;

    botGroundPos = gameData->positions->botGround;
    botGroundVisualPos = gameData->positions->botGround;
    topGroundPos = gameData->positions->topGround;
    topGroundVisualPos = gameData->positions->topGround;

    backgroundColor = data->backgroundColor;
    groundColor = data->groundColor;
    lineColor = data->lineColor;
}

void Level::initItems() {
    for (BufferedBlock* bBlock : data->blocks) items.push_back(new Block(game, bBlock));
    for (BufferedTrigger* bTrigger : data->triggers) items.push_back(new Trigger(game, bTrigger));
    
    std::sort(items.begin(), items.end(), &sortItems);
}

void Level::initConfig() {
    speed = data->startpos.speed;

    if (items.size() > 0) {
        Item* lastItem = items[items.size() - 1];
        Block* block = ItemManager::castToBlock(lastItem);
        Trigger* trigger = ItemManager::castToTrigger(lastItem);

        if (block) levelLength = block->getData()->pos.x + gameData->sizes->levelEndPadding;
        else if (trigger) levelLength = trigger->getData()->pos.x + gameData->sizes->levelEndPadding;

    } else levelLength = gameData->sizes->levelEndPadding;
}

// CLEANUP
Level::~Level() noexcept(false) {
    static H2DE_Engine* engine = game->getEngine(); 

    saveData();

    if (player) delete player;
    if (data) delete data;

    H2DE_DestroyTimelineManager(topGroundTM);
    H2DE_DestroyTimelineManager(botGroundTM);

    game->getCamera()->clearTM();
    game->getMegahack()->resetHitboxTrail();

    H2DE_PlaySFX(engine, "exit-level.ogg", 0);
}

void Level::saveData() {
    std::string SAVESpath = "data/saves.json";
    std::string stingifiedID = std::to_string(data->id);
    json* saves = H2DE_Json::read(SAVESpath);

    if ((*saves)["levels"].contains(stingifiedID)) {
        json level = (*saves)["levels"][stingifiedID];
        int savedJumps = level["jumps"];
        int savedClicks = level["clicks"];

        (*saves)["levels"][stingifiedID]["attempts"] = attempts;
        (*saves)["levels"][stingifiedID]["jumps"] = savedJumps + player->getJumps();
        (*saves)["levels"][stingifiedID]["clicks"] = savedClicks + player->getClicks();

    } else {
        (*saves)["levels"][stingifiedID] = {};
        (*saves)["levels"][stingifiedID]["attempts"] = attempts;
        (*saves)["levels"][stingifiedID]["jumps"] = player->getJumps();
        (*saves)["levels"][stingifiedID]["clicks"] = player->getClicks();

        (*saves)["levels"][stingifiedID]["progress"] = {};
        (*saves)["levels"][stingifiedID]["coins"] = {};
    }

    (*saves)["levels"][stingifiedID]["progress"]["normal"] = 0;
    (*saves)["levels"][stingifiedID]["progress"]["practice"] = 0;

    (*saves)["levels"][stingifiedID]["coins"][0] = coins[0];
    (*saves)["levels"][stingifiedID]["coins"][1] = coins[1];
    (*saves)["levels"][stingifiedID]["coins"][2] = coins[2];

    (*saves)["levels"][stingifiedID]["progress"]["normal"] = bestNormalMode;
    (*saves)["levels"][stingifiedID]["progress"]["practice"] = bestPracticeMode;

    if (!H2DE_Json::write(SAVESpath, saves, 4)) {
        throw std::runtime_error("HGD-3002: Error saving player data => Writing player data failed");
    }
}

// UPDATE
void Level::update() {
    static Camera* camera = game->getCamera();
    GameState state = game->getState();

    switch (state.main) {
        case LEVEL_PLAYING:
            H2DE_TickTimelineManager(topGroundTM);
            H2DE_TickTimelineManager(botGroundTM);
            for (Item* item : items) item->update();
            player->update();
            updateBackground();
            camera->update();
            break;
    }
}

void Level::updateBackground() {
    static GameData* gameData = game->getData();

    backgroundPos.x += (gameData->physics->speeds[speed] * gameData->physics->backgroundRatio);
    updateBackgroundY();
}

void Level::updateBackgroundY() {
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();

    static float camMinY = gameData->positions->cameraMinY;
    static float camMaxY = gameData->positions->cameraMaxY;
    static float bgMinY = gameData->positions->backgroundMinY;
    static float bgMaxY = gameData->positions->backgroundMaxY;

    float camPosY = camera->getPos().y;

    backgroundPos.y = ((camPosY - camMinY) / (camMaxY - camMinY)) * (bgMaxY - bgMinY) + bgMinY;
}

// RENDER
void Level::render() {
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();

    H2DE_Size absGroundSize = calculator->convertToPx(gameData->sizes->ground);
    H2DE_Size absLineSize = calculator->convertToPx(gameData->sizes->line);
    int groundIndex = Zindex{ G, 0 }.getIndex();
    int lineIndex = Zindex{ G, 1 }.getIndex();

    // render background
    H2DE_GraphicObject* background = H2DE_CreateGraphicObject();
    background->type = IMAGE;
    background->pos = calculator->convertToPx(backgroundPos, gameData->sizes->background, false, false);
    background->size = calculator->convertToPx(gameData->sizes->background);
    background->texture = data->backgroundTexture;
    background->repeatX = true;
    background->color = static_cast<H2DE_Color>(backgroundColor);
    background->index = Zindex{ BG, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, background);

    // render bot ground
    H2DE_GraphicObject* botGround = H2DE_CreateGraphicObject(*background);
    botGround->pos = calculator->convertToPx(botGroundVisualPos, gameData->sizes->ground, false, false);
    botGround->size = absGroundSize;
    botGround->texture = data->groundTexture;
    botGround->color = static_cast<H2DE_Color>(groundColor);
    botGround->index = groundIndex;
    H2DE_AddGraphicObject(engine, botGround);

    // render bot line
    H2DE_GraphicObject* botLine = H2DE_CreateGraphicObject(*background);
    botLine->pos = calculator->convertToPx({ gameData->offsets->botLine.x, botGroundVisualPos.y + gameData->offsets->botLine.y }, gameData->sizes->line, true, false);
    botLine->size = absLineSize;
    botLine->texture = data->lineTexture;
    botLine->repeatX = false;
    botLine->color = static_cast<H2DE_Color>(lineColor);
    botLine->index = lineIndex;
    H2DE_AddGraphicObject(engine, botLine);

    // render top ground
    H2DE_GraphicObject* topGround = H2DE_CreateGraphicObject(*botGround);
    topGround->pos = calculator->convertToPx(topGroundVisualPos, gameData->sizes->ground, false, false);
    topGround->flip = SDL_FLIP_VERTICAL;
    H2DE_AddGraphicObject(engine, topGround);

    // render top line
    H2DE_GraphicObject* topLine = H2DE_CreateGraphicObject(*botLine);
    topLine->pos = calculator->convertToPx({ gameData->offsets->topLine.x, topGroundVisualPos.y + gameData->offsets->topLine.y }, gameData->sizes->line, true, false);
    H2DE_AddGraphicObject(engine, topLine);

    for (Item* item : items) item->render();
    player->render();
}

// STATIC
bool Level::sortItems(Item* item1, Item* item2) {
    Block* block1 = ItemManager::castToBlock(item1);
    Trigger* trigger1 = ItemManager::castToTrigger(item1);

    Block* block2 = ItemManager::castToBlock(item2);
    Trigger* trigger2 = ItemManager::castToTrigger(item2);

    float posX1 = (block1) ? block1->getData()->pos.x : trigger1->getData()->pos.x;
    float posX2 = (block2) ? block2->getData()->pos.x : trigger2->getData()->pos.x;

    return (posX1 < posX2);
}

// GETTER
LevelData* Level::getData() const {
    return data;
}

LevelPos Level::getBotGroundPos() const {
    return botGroundPos;
}

LevelPos Level::getTopGroundPos() const {
    return topGroundPos;
}

Color Level::getBackgroundColor() const {
    return backgroundColor;
}

Color Level::getGroundColor() const {
    return groundColor;
}

Color Level::getLineColor() const {
    return lineColor;
}

Player* Level::getPlayer() const {
    return player;
}

std::vector<Item*>* Level::getItems() {
    return &items;
}

int Level::getCurrentSpeed() const {
    return speed;
}

int Level::getLevelLength() const {
    return levelLength;
}

LevelMode Level::getMode() const {
    return mode;
}

// SETTER
void Level::setBackgroundColor(Color color) {
    backgroundColor.r = color.r;
    backgroundColor.g = color.g;
    backgroundColor.b = color.b;
}

void Level::setGroundColor(Color color) {
    groundColor.r = color.r;
    groundColor.g = color.g;
    groundColor.b = color.b;
}

void Level::setLineColor(Color color) {
    lineColor.r = color.r;
    lineColor.g = color.g;
    lineColor.b = color.b;
}

void Level::setTopGroundPos(LevelPos pos, unsigned int ms) {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_ClearTimelineManager(topGroundTM);

    topGroundPos.x = pos.x;
    topGroundPos.y = pos.y;

    if (ms != 0) {
        LevelPos defaultPos = topGroundVisualPos;

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, ms, EASE_IN_OUT, [this, defaultPos, pos](float blend) {
            this->topGroundVisualPos.y = defaultPos.y + blend * (pos.y - defaultPos.y);
        }, NULL, 0);
        H2DE_AddTimelineToManager(topGroundTM, t);
    } else topGroundVisualPos.y = topGroundPos.y;
}

void Level::setBotGroundPos(LevelPos pos, unsigned int ms) {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_ClearTimelineManager(botGroundTM);

    botGroundPos.x = pos.x;
    botGroundPos.y = pos.y;

    if (ms != 0) {
        LevelPos defaultPos = botGroundVisualPos;

        H2DE_Timeline* t = H2DE_CreateTimeline(engine, ms, EASE_IN_OUT, [this, defaultPos, pos](float blend) {
            this->botGroundVisualPos.y = defaultPos.y + blend * (pos.y - defaultPos.y);
        }, NULL, 0);
        H2DE_AddTimelineToManager(botGroundTM, t);
    } else botGroundVisualPos.y = botGroundPos.y;
}

void Level::setMode(LevelMode m) {
    static H2DE_Engine* engine = game->getEngine();
    static std::vector<std::string> modes = { "normal", "practice" };
    
    mode = m;
    if (mode == PRACTICE_MODE) H2DE_PlaySong(engine, "stay-inside-me.mp3", -1);
    H2DE_PauseSong(engine);

    std::cout << "Now in " << modes[mode] << " mode" << std::endl;
}

void Level::setBestNormalMode(float percentage) {
    if (bestNormalMode < percentage) bestNormalMode = percentage;
}

void Level::setBestPracticeMode(float percentage) {
    if (bestPracticeMode < percentage) bestPracticeMode = percentage;
}

// OTHER
void Level::finish() {
    static H2DE_Engine* engine = game->getEngine();

    game->setState({ LEVEL_END, DEFAULT });
    H2DE_PauseSong(engine);

    if (mode == NORMAL_MODE) bestNormalMode = 100.0f;
    else bestPracticeMode = 100.0f;

    if (mode == NORMAL_MODE && data->startpos.playerPos.x == 0) {
        int coinNb = 0;

        for (Item* item : items) {
            Block* block = ItemManager::castToBlock(item);
            if (!block) continue;

            BufferedBlock* bData = block->getData();
            if (bData->specialData.desc != SD_SECRET) continue;

            if (!coins[coinNb]) coins[coinNb] = block->isPickedUp();
            coinNb++;
        }
    }
}

void Level::pause() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_PauseSong(engine);
    game->setState({ LEVEL_PAUSE, DEFAULT });
}

void Level::resume() {
    static H2DE_Engine* engine = game->getEngine();
    
    H2DE_ResumeSong(engine);
    game->setState({ LEVEL_PLAYING, DEFAULT });
}

void Level::respawn() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();

    H2DE_ClearTimelineManager(botGroundTM);
    H2DE_ClearTimelineManager(topGroundTM);

    Checkpoint* lastPracticeCheckpoint = player->getLastPracticeCheckpoint();

    if (mode == NORMAL_MODE || (mode == PRACTICE_MODE && lastPracticeCheckpoint == nullptr)) {
        camera->reset();
        speed = data->startpos.speed;

        backgroundPos = gameData->positions->background;
        botGroundPos = gameData->positions->botGround;
        botGroundVisualPos = gameData->positions->botGround;
        topGroundPos = gameData->positions->topGround;
        topGroundVisualPos = gameData->positions->topGround;

        backgroundColor = data->backgroundColor;
        groundColor = data->groundColor;
        lineColor = data->lineColor;

        player->reset(&(data->startpos));

        if (mode == NORMAL_MODE) H2DE_PlaySong(engine, data->song, 0);

    } else if (mode == PRACTICE_MODE) {
        camera->setPos(lastPracticeCheckpoint->camPos, 0);
        speed = lastPracticeCheckpoint->speed;

        player->reset(lastPracticeCheckpoint);
    }

    for (Item* item : items) item->reset();

    game->getMegahack()->resetHitboxTrail();

    updateBackgroundY();
    game->setState({ LEVEL_PLAYING, DEFAULT });

    attempts++;
    std::cout << "Attempt " << attempts << std::endl;
}
