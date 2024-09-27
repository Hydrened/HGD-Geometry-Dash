#include "level.h"

// LEVELS

// PRIVATE
void Level::Levels::getMenuInfos(LevelData* data, json* level) {
    json menuInfos = (*level)["menuInfos"];
    json meColor = menuInfos["backgroundColor"];

    data->name = menuInfos["name"];
    data->difficulty = menuInfos["difficulty"];
    data->menuColor = { static_cast<Uint8>(meColor["r"]), static_cast<Uint8>(meColor["g"]), static_cast<Uint8>(meColor["b"]), 255 };
}

void Level::Levels::getConfigInfos(LevelData* data, json* level)  {
    json config = (*level)["config"];

    json bgCol = config["background"]["color"];
    data->backgroundColor = { static_cast<Uint8>(bgCol["r"]), static_cast<Uint8>(bgCol["g"]), static_cast<Uint8>(bgCol["b"]), 255 };
    json grCol = config["ground"]["color"];
    data->groundColor = { static_cast<Uint8>(grCol["r"]), static_cast<Uint8>(grCol["g"]), static_cast<Uint8>(grCol["b"]), 255 };
    json liCol = config["line"]["color"];
    data->lineColor = { static_cast<Uint8>(liCol["r"]), static_cast<Uint8>(liCol["g"]), static_cast<Uint8>(liCol["b"]), 255 };

    data->backgroundTexture = config["background"]["texture"];
    data->groundTexture = config["ground"]["texture"];
    data->lineTexture = config["line"]["texture"];

    
    for (int i = 0; i < config["colors"].size(); i++) {
        json col = config["colors"][i];
        data->colors.push_back({ static_cast<Uint8>(col["r"]), static_cast<Uint8>(col["g"]), static_cast<Uint8>(col["b"]), 255 });
    }

    data->song = config["song"];
}

void Level::Levels::getBlocksInfos(LevelData* data, json* level, json* itemsData)  {
    for (json block : (*level)["blocks"]) {
        json itemData = (*itemsData)[block["i"]];
        BufferedBlock* bBlock = new BufferedBlock();
        bBlock->type = itemData["type"];

        bBlock->texture = std::string(block["i"]) + ".png";

        json pos = block["p"];
        bBlock->pos = { pos["x"], pos["y"] };
        json texSize = itemData["size"]["texture"];
        bBlock->textureSize = { texSize["w"], texSize["h"] };
        json texOffset = itemData["offset"]["texture"];
        bBlock->textureOffset = { texOffset["x"], texOffset["y"] };
        
        if (block.contains("r")) bBlock->rotation = block["r"];
        json hitSize = itemData["size"]["hitbox"];
        json hitOffset = itemData["offset"]["hitbox"];
        switch (static_cast<int>(bBlock->rotation)) {
            case 90:
                bBlock->hitboxSize = { hitSize["h"], hitSize["w"] };
                bBlock->hitboxOffset = { hitOffset["y"], hitOffset["x"] };
                break;
            case 180:
                bBlock->hitboxSize = { hitSize["w"], hitSize["h"] };
                bBlock->hitboxOffset = { 1.0f - static_cast<float>(hitSize["w"]) - static_cast<float>(hitOffset["x"]), 1.0f - static_cast<float>(hitSize["h"]) - static_cast<float>(hitOffset["y"]) };
                break;
            case 270:
                bBlock->hitboxSize = { hitSize["h"], hitSize["w"] };
                bBlock->hitboxOffset = { 1.0f - static_cast<float>(hitSize["h"]) - static_cast<float>(hitOffset["y"]), 1.0f - static_cast<float>(hitSize["w"]) - static_cast<float>(hitOffset["x"]) };
                break;
            default:
                bBlock->hitboxSize = { hitSize["w"], hitSize["h"] };
                bBlock->hitboxOffset = { hitOffset["x"], hitOffset["y"] };
                break;
        }

        if (block.contains("c")) bBlock->colorID = block["c"];

        if (block.contains("f")) {
            bool fx = (std::string(block["f"]).find("x") != std::string::npos);
            bool fy = (std::string(block["f"]).find("y") != std::string::npos);

            bBlock->flip = (fx && fy) ? SDL_FLIP_NONE : (fx) ? SDL_FLIP_HORIZONTAL : (fy) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        } else bBlock->flip = SDL_FLIP_NONE;

        bBlock->zIndex = new Zindex(Zindex::getLayer(itemData["zIndex"]["layer"]), itemData["zIndex"]["order"]);
        if (itemData.contains("special")) bBlock->specialData = itemData["special"];

        data->blocks.push_back(bBlock);
    }
}

void Level::Levels::getTriggersInfos(LevelData* data, json* level, json* itemsData) {
    for (json trigger : (*level)["triggers"]) {
        BufferedTrigger* bTrigger = new BufferedTrigger();
        json itemData = (*itemsData)[trigger["i"]];

        if (itemData["type"] != STARTPOS) {
            bTrigger->type = itemData["type"];
            bTrigger->pos = { trigger["p"]["x"], trigger["p"]["y"] };
            bTrigger->color = { static_cast<Uint8>(trigger["c"]["r"]), static_cast<Uint8>(trigger["c"]["g"]), static_cast<Uint8>(trigger["c"]["b"]) };
            bTrigger->ms = trigger["d"];
            if (trigger.contains("tt")) bTrigger->touchTrigger = trigger["tt"];

            data->triggers.push_back(bTrigger);
        }
    }
}

void Level::Levels::getStartposInfo(LevelData* data, json* level, json* itemsData) {
    static GameData* gameData = new GameData();
    json config = (*level)["config"];
    Checkpoint* startpos = nullptr;

    for (json trigger : (*level)["triggers"]) if ((*itemsData)[trigger["i"]]["type"] == STARTPOS && trigger["p"]["x"] >= 0) {
        if (!startpos) startpos = new Checkpoint();
        
        if (startpos->playerPos.x <= trigger["p"]["x"]) {
            startpos->camPos = { (float)(trigger["p"]["x"]) + gameData->positions->camera.x, (float)(trigger["p"]["y"]) - 10.0f + gameData->sizes->cameraPaddingTop};
            startpos->playerPos = { trigger["p"]["x"], trigger["p"]["y"] };
            startpos->velocity = { gameData->physics->speeds[trigger["sp"]], 0 };
            startpos->speed = trigger["sp"];
            startpos->gravity = trigger["gr"];
            startpos->size = trigger["si"];
            startpos->gamemode = trigger["ga"];
        }
    }

    if (!startpos) {
        startpos = new Checkpoint();
        startpos->camPos = gameData->positions->camera;
        startpos->velocity = { gameData->physics->speeds[config["speed"]], 0 };
        startpos->speed = config["speed"];
        startpos->gravity = config["gravity"];
        startpos->size = config["size"];
        startpos->gamemode = config["gamemode"];
    }

    data->startpos = *startpos;
}

// PUBLIC
LevelData* Level::Levels::getLevelData(int id) {
    Uint32 start = SDL_GetTicks();
    std::cout << "Loading level data..." << std::endl;

    LevelData* data = new LevelData();
    json* levels = H2DE_Json::read("data/levels.json");

    std::string stingifiedID = std::to_string(id);
    if ((*levels).contains(stingifiedID)) {

        json* level = &(*levels)[std::to_string(id)];
        try {
            data->id = id;
            getMenuInfos(data, level);
            getConfigInfos(data, level);

            json* itemsData = H2DE_Json::read("data/items.json");
            getBlocksInfos(data, level, itemsData);
            getTriggersInfos(data, level, itemsData);
            getStartposInfo(data, level, itemsData);

        } catch (const std::exception& e) {
            throw std::runtime_error("Error loading level => " + std::string(e.what()));
        }
    } else throw std::runtime_error("Error loading level => Level ID not found");

    std::cout << "Level data loaded in " << SDL_GetTicks() - start << "ms" << std::endl;
    return data;
}

void Level::Levels::loadLevelData(int id, const fs::path& gmdFile) {

}



// LEVEL

// INIT
Level::Level(Game* g, int i) : game(g), id(i) {
    static H2DE_Engine* engine = game->getEngine();    

    std::cout << "Initializing level..." << std::endl;
    Uint32 start = SDL_GetTicks();
    data = Levels::getLevelData(id);

    initAttempts();
    initCamera();
    initLevelElements();
    initItems();
    initConfig();

    player = new Player(game, &data->startpos);
    std::cout << "Level initialized in " << SDL_GetTicks() - start << "ms" << std::endl;

    H2DE_PlaySFX(engine, "test.wav", 0); // replace => test.wav(playing level sound)
    game->setState({ LEVEL_STARTING_DELAY, DEFAULT });
    std::cout << "Attempt " << attempts << std::endl;

    Game::delay(1000, [this]() {
        game->setState({ LEVEL_PLAYING, DEFAULT });
        H2DE_PlaySong(engine, data->song, 0);
    }); // error => rare crash
}

void Level::initAttempts() {
    json* saves = H2DE_Json::read("data/saves.json");
    std::string stingifiedID = std::to_string(id);
    if ((*saves)["levels"].contains(stingifiedID)) {
        attempts = static_cast<int>((*saves)["levels"][stingifiedID]["attempts"]) + 1;
    } else throw std::runtime_error("Error loading level => Attempts could not be loaded");
}

void Level::initCamera() {
    game->getCamera()->globalSet(data->startpos.camPos);
}

void Level::initLevelElements() {
    backgroundPos = gameData->positions->backgroundPos;
    botGroundPos = gameData->positions->botGroundPos;
    topGroundPos = gameData->positions->topGroundPos;
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

    Item* lastItem = items[items.size() - 1];
    Block* block = ItemManager::castToBlock(lastItem);
    Trigger* trigger = ItemManager::castToTrigger(lastItem);
    if (block) levelLength = block->getData()->pos.x + gameData->sizes->levelEndPadding;
    else if (trigger) levelLength = trigger->getData()->pos.x + gameData->sizes->levelEndPadding;
}

// CLEANUP
Level::~Level() {
    if (data) delete data;
    if (player) delete player;
}

// UPDATE
void Level::update() {
    GameState state = game->getState();

    switch (state.main) {
        case LEVEL_PLAYING:
            backgroundPos.x += (gameData->physics->speeds[speed] * gameData->physics->backgroundRatio);
            player->update(); break;
            break;
    }
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
    H2DE_GraphicObject* background = new H2DE_GraphicObject();
    background->type = IMAGE;
    background->pos = calculator->convertToPx(backgroundPos, gameData->sizes->background, false, false);
    background->size = calculator->convertToPx(gameData->sizes->background);
    background->texture = "background_1.png";
    background->repeatX = true;
    background->color = static_cast<H2DE_Color>(backgroundColor);
    background->index = Zindex{ BG, 0 }.getIndex();
    H2DE_AddGraphicObject(engine, background);

    // render bot ground
    H2DE_GraphicObject* botGround = new H2DE_GraphicObject();
    botGround->type = IMAGE;
    botGround->pos = calculator->convertToPx(botGroundPos, gameData->sizes->ground, false, false);
    botGround->size = absGroundSize;
    botGround->texture = "ground_1.png";
    botGround->repeatX = true;
    botGround->color = static_cast<H2DE_Color>(groundColor);
    botGround->index = groundIndex;
    H2DE_AddGraphicObject(engine, botGround);

    // render bot line
    H2DE_GraphicObject* botLine = new H2DE_GraphicObject();
    botLine->type = IMAGE;
    botLine->pos = calculator->convertToPx({ gameData->offsets->botLine.x, botGroundPos.y + gameData->offsets->botLine.y }, gameData->sizes->line, true, false);
    botLine->size = absLineSize;
    botLine->texture = "line_1.png";
    botLine->color = static_cast<H2DE_Color>(lineColor);
    botLine->index = lineIndex;
    H2DE_AddGraphicObject(engine, botLine);

    // render top ground
    H2DE_GraphicObject* topGround = new H2DE_GraphicObject();
    topGround->type = IMAGE;
    topGround->pos = calculator->convertToPx(topGroundPos, gameData->sizes->ground, false, false);
    topGround->size = absGroundSize;
    topGround->texture = "ground_1.png";
    topGround->repeatX = true;
    topGround->color = static_cast<H2DE_Color>(groundColor);
    topGround->flip = SDL_FLIP_VERTICAL;
    topGround->index = groundIndex;
    H2DE_AddGraphicObject(engine, topGround);

    // render top line
    H2DE_GraphicObject* topLine = new H2DE_GraphicObject();
    topLine->type = IMAGE;
    topLine->pos = calculator->convertToPx({ gameData->offsets->topLine.x, topGroundPos.y + gameData->offsets->topLine.y }, gameData->sizes->line, true, false);
    topLine->size = absLineSize;
    topLine->texture = "line_1.png";
    topLine->color = static_cast<H2DE_Color>(lineColor);
    topLine->index = lineIndex;
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
LevelData* Level::getData() {
    return data;
}

int Level::getCurrentSpeed() {
    return speed;
}

LevelPos Level::getBotGroundPos() {
    return botGroundPos;
}

LevelPos Level::getTopGroundPos() {
    return topGroundPos;
}

Player* Level::getPlayer() {
    return player;
}

std::vector<Item*>* Level::getItems() {
    return &items;
}

int Level::getLevelLength() {
    return levelLength;
}

// OTHER
void Level::finish() {
    static H2DE_Engine* engine = game->getEngine();

    game->setState({ LEVEL_END, DEFAULT });
    H2DE_PauseSong(engine);
}

void Level::pause() {
    game->setState({ LEVEL_PAUSE, DEFAULT });
}

void Level::resume() {
    game->setState({ LEVEL_PLAYING, DEFAULT });
}
