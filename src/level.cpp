#include "level.h"

// INIT
Level::Level(Game* g, int i) : game(g), id(i) {
    static H2DE_Engine* engine = game->getEngine(); 

    std::cout << "Initializing level..." << std::endl;
    Uint32 start = SDL_GetTicks();
    data = LevelLoader::getLevelData(game, id);

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
        H2DE_PlaySound(engine, 0, data->song, 0);
    });
}

void Level::initFromSave() {
    json* saves = H2DE_Json::read("data/saves.json");
    std::string stingifiedID = std::to_string(id);

    if ((*saves)["levels"].contains(stingifiedID)) {
        attempts = (int)((*saves)["levels"][stingifiedID]["attempts"]) + 1;
        for (bool c : (*saves)["levels"][stingifiedID]["coins"]) {
            savedCoins.push_back(c);
            coins.push_back(c);
        }
        bestNormalMode = (*saves)["levels"][stingifiedID]["progress"]["normal"];
        bestPracticeMode = (*saves)["levels"][stingifiedID]["progress"]["practice"];

    } else {
        attempts = 1;
        savedCoins = { false, false, false };
        coins = { false, false, false };
        bestNormalMode = 0.0f;
        bestPracticeMode = 0.0f;
    }
}

void Level::initCamera() {
    game->getCamera()->setPos(data->startpos.camPos, 0);
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
    for (BufferedBlock* bb : data->blocks) items.push_back(new Block(game, bb));
    for (BufferedTrigger* bTrigger : data->triggers) items.push_back(new Trigger(game, bTrigger));
    
    std::sort(items.begin(), items.end(), &sortItems);
}

void Level::initConfig() {
    speed = data->startpos.speed;

    if (items.size() > 0) {
        Item* lastItem = items[items.size() - 1];
        Block* block = ItemManager::castToBlock(lastItem);
        Trigger* trigger = ItemManager::castToTrigger(lastItem);

        if (block) levelLength = block->getBufferedData()->pos.x;
        else if (trigger) levelLength = trigger->getBufferedData()->pos.x;

    } else levelLength = gameData->sizes->levelMinLength;
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

    if (finished) {
        (*saves)["levels"][stingifiedID]["coins"][0] = coins[0];
        (*saves)["levels"][stingifiedID]["coins"][1] = coins[1];
        (*saves)["levels"][stingifiedID]["coins"][2] = coins[2];
    }

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
        default: break;
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
    renderElements();
    if (mode == PRACTICE_MODE) renderPracticeUI();

    for (Item* item : items) item->render();
    player->render();
}

void Level::renderElements() {
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();
    static Camera* camera = game->getCamera();

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
    background->rgb = backgroundColor;
    background->index = Zindex{ BG, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, background);

    // render bot ground
    H2DE_GraphicObject* botGround = H2DE_CreateGraphicObject(*background);
    botGround->pos = calculator->convertToPx(botGroundVisualPos, gameData->sizes->ground, false, false);
    botGround->size = absGroundSize;
    botGround->texture = data->groundTexture;
    botGround->rgb = groundColor;
    botGround->index = groundIndex;
    H2DE_AddGraphicObject(engine, botGround);

    // render bot line
    H2DE_GraphicObject* botLine = H2DE_CreateGraphicObject(*background);
    botLine->pos = calculator->convertToPx({ gameData->offsets->botLine.x, botGroundVisualPos.y + gameData->offsets->botLine.y }, gameData->sizes->line, true, false);
    botLine->size = absLineSize;
    botLine->texture = data->lineTexture;
    botLine->repeatX = false;
    botLine->rgb = lineColor;
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

    float wallXpos = (float)levelLength + gameData->offsets->levelEndWallX + gameData->sizes->camera.w + gameData->positions->camera.x;
    if (camera->isOnScreen({ wallXpos - 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f })) {
        H2DE_Size wallSize = calculator->convertToPx(LevelSize{ 1.0f, 1.0f });
        H2DE_Pos center = calculator->convertToPx(LevelOffset{ 0.5f, 0.5f });

        for (int i = 0; i < gameData->sizes->levelHeight; i++) {
            H2DE_GraphicObject* wall = H2DE_CreateGraphicObject();
            wall->type = IMAGE;
            wall->texture = "1_3.png";
            wall->pos = calculator->convertToPx({ wallXpos, 1.0f * i }, { 1.0f, 1.0f }, false, false);
            wall->size = wallSize;
            wall->rotationOrigin = center;
            wall->rotation = 270.0f;
            wall->index = Zindex{ T1, 20 }.getIndex();
            H2DE_AddGraphicObject(engine, wall);
        }
    }
}

void Level::renderPracticeUI() {
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    static Calculator* calculator = game->getCalculator();

    H2DE_GraphicObject* practiceButtons = H2DE_CreateGraphicObject();
    practiceButtons->type = IMAGE;
    practiceButtons->pos = calculator->convertToPx(gameData->positions->practiceButtons, gameData->sizes->practiceButtons, true, true);
    practiceButtons->size = calculator->convertToPx(gameData->sizes->practiceButtons);
    practiceButtons->texture = "practice-buttons.png";
    practiceButtons->index = Zindex{ UI, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, practiceButtons);
}

// STATIC
bool Level::sortItems(Item* item1, Item* item2) {
    Block* block1 = ItemManager::castToBlock(item1);
    Trigger* trigger1 = ItemManager::castToTrigger(item1);

    Block* block2 = ItemManager::castToBlock(item2);
    Trigger* trigger2 = ItemManager::castToTrigger(item2);

    float posX1 = (block1) ? block1->getBufferedData()->pos.x : trigger1->getBufferedData()->pos.x;
    float posX2 = (block2) ? block2->getBufferedData()->pos.x : trigger2->getBufferedData()->pos.x;

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

BlockEffect Level::getBlockEffect() const {
    return currentBlockEffect;
}

std::vector<bool> Level::getSavedCoins() const {
    return savedCoins;
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
            this->topGroundVisualPos.y = lerp(defaultPos.y, pos.y, blend);
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
            this->botGroundVisualPos.y = lerp(defaultPos.y, pos.y, blend);
        }, NULL, 0);
        H2DE_AddTimelineToManager(botGroundTM, t);
    } else botGroundVisualPos.y = botGroundPos.y;
}

void Level::setMode(LevelMode m) {
    static H2DE_Engine* engine = game->getEngine();
    static std::vector<std::string> modes = { "normal", "practice" };
    
    mode = m;
    if (mode == PRACTICE_MODE) H2DE_PlaySound(engine, 0, "stay-inside-me.mp3", -1);
    H2DE_PauseSound(engine, 0);

    std::cout << "Now in " << modes[mode] << " mode" << std::endl;
}

void Level::setBestNormalMode(float percentage) {
    if (bestNormalMode < percentage) bestNormalMode = percentage;
}

void Level::setBestPracticeMode(float percentage) {
    if (bestPracticeMode < percentage) bestPracticeMode = percentage;
}

void Level::setBlockEffect(BlockEffect effect) {
    currentBlockEffect = effect;
}

void Level::gotCoinNb(unsigned int nb) {
    if (nb < 3) coins[nb] = true;
}

// OTHER
void Level::finish() {
    static H2DE_Engine* engine = game->getEngine();

    game->setState({ LEVEL_END, DEFAULT });

    if (mode == NORMAL_MODE) bestNormalMode = 100.0f;
    else bestPracticeMode = 100.0f;

    if (mode == NORMAL_MODE && data->startpos.playerPos.x == 0) finished = true;
}

void Level::pause() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_PauseSound(engine, -1);
    game->setState({ LEVEL_PAUSE, DEFAULT });
}

void Level::resume() {
    static H2DE_Engine* engine = game->getEngine();
    
    H2DE_ResumeSound(engine, -1);
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
        camera->reset(data->startpos.camPos);
        speed = data->startpos.speed;

        backgroundPos = gameData->positions->background;
        botGroundPos = gameData->positions->botGround;
        botGroundVisualPos = gameData->positions->botGround;
        topGroundPos = gameData->positions->topGround;
        topGroundVisualPos = gameData->positions->topGround;

        player->reset(&(data->startpos));

        if (mode == NORMAL_MODE) H2DE_PlaySound(engine, 0, data->song, 0);

    } else if (mode == PRACTICE_MODE) {
        camera->setPos(lastPracticeCheckpoint->camPos, 0);
        speed = lastPracticeCheckpoint->speed;

        player->reset(lastPracticeCheckpoint);
    }

    currentBlockEffect = FADE;
    
    backgroundColor = data->backgroundColor;
    groundColor = data->groundColor;
    lineColor = data->lineColor;

    for (Item* item : items) item->reset();
    if (mode == NORMAL_MODE && !finished) for (int i = 0; i < 3; i++) coins[i] = savedCoins[i];
    refreshCoins();

    game->getMegahack()->resetHitboxTrail();

    updateBackgroundY();
    game->setState({ LEVEL_PLAYING, DEFAULT });
    H2DE_ResumeSound(engine, -1);

    attempts++;
    std::cout << "Attempt " << attempts << std::endl;
}

void Level::refreshCoins() {
    int coinIndex = 0;
    for (Item* item : items) {
        Block* block = ItemManager::castToBlock(item);
        if (!block) continue;

        if (block->getBufferedData()->id == "0_1") {
            block->setCoinIndex(coinIndex);
            if (coins[coinIndex]) block->setPickedUp(true);
            coinIndex++;
        }
    }
}
