#include "level_loader.h"

// MAIN
LevelData* LevelLoader::getLevelData(Game* game, int id) {
    Uint32 start = SDL_GetTicks();
    std::cout << "Loading level data..." << std::endl;

    LevelData* data = new LevelData();
    json* level = H2DE_Json::read("data/levels/" + std::to_string(id) + ".gdl");

    try {
        data->id = id;
        getMenuInfos(game, data, level);
        getConfigInfos(game, data, level);
        getBlocksInfos(game, data, level);
        getTriggersInfos(game, data, level);
        getStartposInfo(game, data, level);

    } catch (const std::exception& e) {
        throw std::runtime_error("HGD-1004: Error loading level data => " + std::string(e.what()));
    }

    std::cout << "Level data loaded in " << SDL_GetTicks() - start << "ms" << std::endl;
    return data;
}

void LevelLoader::loadLevelData(const fs::path& gmdFile) {
    if (gmdFile.extension() == ".gmd") {
        Uint32 start = SDL_GetTicks();
        std::cout << "Loading extern level data..." << std::endl;

        std::ifstream file(gmdFile);
        if (!file.is_open()) throw std::runtime_error("HGD-2003: Error loading extern level => File can't be opened");
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();

        int id;
        if (!containsK(content, "k1")) {
            id = -1; // replace => not uploaded level (give special id)
        } else id = std::stoi(getK(content, "k1"));

        initLevelsInfos(content, id);
        initSaveInfos(content, id);

        std::cout << "Level data loaded in " << SDL_GetTicks() - start << "ms" << std::endl;
    } else throw std::runtime_error("HGD-2002: Error loading extern level => File isn't a gmd file");
}

// GET DATA
void LevelLoader::getMenuInfos(Game* game, LevelData* data, json* level) {
    json menuInfos = (*level)["menuInfos"];
    json meColor = menuInfos["backgroundColor"];

    data->name = menuInfos["name"];
    data->difficulty = menuInfos["difficulty"];
    data->menuColor = { (Uint8)(meColor["r"]), (Uint8)(meColor["g"]), (Uint8)(meColor["b"]), SDL_MAX_UINT8 };
}

void LevelLoader::getConfigInfos(Game* game, LevelData* data, json* level)  {
    json config = (*level)["config"];

    json bgCol = config["background"]["color"];
    data->backgroundColor = { (Uint8)(bgCol["r"]), (Uint8)(bgCol["g"]), (Uint8)(bgCol["b"]), SDL_MAX_UINT8 };
    json grCol = config["ground"]["color"];
    data->groundColor = { (Uint8)(grCol["r"]), (Uint8)(grCol["g"]), (Uint8)(grCol["b"]), SDL_MAX_UINT8 };
    json liCol = config["line"]["color"];
    data->lineColor = { (Uint8)(liCol["r"]), (Uint8)(liCol["g"]), (Uint8)(liCol["b"]), SDL_MAX_UINT8 };

    data->backgroundTexture = config["background"]["texture"];
    data->groundTexture = config["ground"]["texture"];
    data->lineTexture = config["line"]["texture"];
    
    for (int i = 0; i < 4; i++) {
        if (i < config["colors"].size()) {
            json col = config["colors"][i];
            data->colors.push_back({ (Uint8)(col["r"]), (Uint8)(col["g"]), (Uint8)(col["b"]), SDL_MAX_UINT8 });
        } else data->colors.push_back({ SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_MAX_UINT8, SDL_MAX_UINT8 });
    }

    data->song = config["song"];
}

void LevelLoader::getBlocksInfos(Game* game, LevelData* data, json* level)  {
    static std::unordered_map<std::string, BlockData>* blocksData = &(game->getData()->other->blocks);

    int coins = 0;

    for (json block : (*level)["blocks"]) {
        BlockData* blockData = &blocksData->at(block["i"]);
        BufferedBlock* bb = new BufferedBlock();

        if (block["i"] == "0_1") {
            coins++;
            if (coins > 3) continue;
        }

        bb->id = block["i"];
        bb->data = new BlockData(*blockData);

        json pos = block["p"];
        bb->pos = { pos["x"], pos["y"] };

        if (block.contains("r")) bb->rotation = block["r"];
        while (bb->rotation >= 360) bb->rotation -= 360;
        while (bb->rotation < 0) bb->rotation += 360;

        if (block.contains("f")) {
            bool fx = (std::string(block["f"]).find("x") != std::string::npos);
            bool fy = (std::string(block["f"]).find("y") != std::string::npos);
            bb->flip = (fx && fy) ? SDL_FLIP_NONE : (fx) ? SDL_FLIP_HORIZONTAL : (fy) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
        }

        if (block.contains("c")) {
            if (block["c"].contains("b")) bb->baseColor = block["c"]["b"];
            if (block["c"].contains("d")) bb->detailColor = block["c"]["d"];
        }

        data->blocks.push_back(bb);
    }
}

void LevelLoader::getTriggersInfos(Game* game, LevelData* data, json* level) {
    static std::unordered_map<std::string, TriggerData>* triggersData = &(game->getData()->other->triggers);

    for (json trigger : (*level)["triggers"]) {
        TriggerData* triggerData = &triggersData->at(trigger["i"]);
        BufferedTrigger* bt = new BufferedTrigger();

        bt->id = trigger["i"];
        bt->data = triggerData;

        if (triggerData->type != T_STARTPOS) {
            bt->pos = { trigger["p"]["x"], trigger["p"]["y"] };
            if (trigger.contains("c")) bt->color = { (Uint8)(trigger["c"]["r"]), (Uint8)(trigger["c"]["g"]), (Uint8)(trigger["c"]["b"]) };
            if (trigger.contains("d")) bt->ms = trigger["d"];
            if (trigger.contains("tt")) bt->touchTrigger = trigger["tt"];

            data->triggers.push_back(bt);
        }
    }
}

void LevelLoader::getStartposInfo(Game* game, LevelData* data, json* level) {
    static GameData* gameData = new GameData();
    static std::unordered_map<std::string, TriggerData>* triggersData = &(gameData->other->triggers);
    json config = (*level)["config"];
    Checkpoint* startpos = nullptr;

    for (json trigger : (*level)["triggers"]) if (triggersData->at(trigger["i"]).type == T_STARTPOS && trigger["p"]["x"] >= 0) {
        if (!startpos) startpos = new Checkpoint();
        
        if (startpos->playerPos.x <= trigger["p"]["x"]) {
            startpos->camPos = { (float)(trigger["p"]["x"]) + gameData->positions->camera.x, (float)(trigger["p"]["y"]) - 10.0f + gameData->sizes->cameraPaddingTop};
            startpos->playerPos = { trigger["p"]["x"], trigger["p"]["y"] };
            startpos->velocity = { gameData->physics->speeds[trigger["sp"]], 0 };
            startpos->speed = trigger["sp"];
            startpos->gravity = trigger["gr"];
            startpos->size = trigger["si"];
            startpos->gamemode = trigger["ga"];

            startpos->botGroundPosY = -1.0f;
        }
    }

    if (!startpos) {
        startpos = new Checkpoint();
        startpos->camPos = gameData->positions->camera;
        startpos->playerPos = { 0, 0 };
        startpos->velocity = { gameData->physics->speeds[config["speed"]], 0 };
        startpos->speed = config["speed"];
        startpos->gravity = config["gravity"];
        startpos->size = config["size"];
        startpos->gamemode = config["gamemode"];

        startpos->botGroundPosY = 0.0f;
    }

    data->startpos = *startpos;
}

// INIT DATA
void LevelLoader::initLevelsInfos(std::string content, int id) {
    std::string levelsPATH = "data/levels/" + std::to_string(id) + ".gdl";
    json level;

    std::string name = getK(content, "k2");
    std::string creator = getK(content, "k5");
    int difficulty =  toNumber(getK(content, "k7"));
    level["menuInfos"] = {};
    level["menuInfos"]["name"] = name;
    level["menuInfos"]["creator"] = creator;
    level["menuInfos"]["difficulty"] = difficulty;

    int songID = (containsK(content, "k8")) ? std::stoi(getK(content, "k8")) : toNumber(getK(content, "k45"));
    level["config"] = {};
    level["config"]["song"] = songID;

    if (!H2DE_Json::write(levelsPATH, &level, 4)) {
        throw std::runtime_error("HGD-3004: Error loading extern level => Writing data in levels failed");
    }
}

void LevelLoader::initSaveInfos(std::string content, int id) {
    std::string savesPATH = "data/saves.json";
    json* saves = H2DE_Json::read(savesPATH);
    json levelSave;

    int attempts = (containsK(content, "k18")) ? std::stoi(getK(content, "k18")) : 0;
    int jumps = (containsK(content, "k36")) ? std::stoi(getK(content, "k36")) : 0;
    levelSave["attempts"] = attempts;
    levelSave["jumps"] = jumps;
    levelSave["clicks"] = 0;

    int normalModePercentage = (containsK(content, "k19")) ? std::stoi(getK(content, "k19")) : 0;
    int practiceModePercentage = (containsK(content, "k20")) ? std::stoi(getK(content, "k20")) : 0;
    levelSave["progress"] = {};
    levelSave["progress"]["normal"] = normalModePercentage;
    levelSave["progress"]["practice"] = practiceModePercentage;

    bool coin1 = (containsK(content, "k61")) ? ((getK(content, "k61") == "true") ? true : false) : false;
    bool coin2 = (containsK(content, "k62")) ? ((getK(content, "k62") == "true") ? true : false) : false;
    bool coin3 = (containsK(content, "k63")) ? ((getK(content, "k63") == "true") ? true : false) : false;
    levelSave["coins"] = {};
    levelSave["coins"].push_back(coin1);
    levelSave["coins"].push_back(coin2);
    levelSave["coins"].push_back(coin3);

    (*saves)["levels"][std::to_string(id)] = levelSave;
    if (!H2DE_Json::write(savesPATH, saves, 4)) {
        throw std::runtime_error("HGD-3005: Error loading extern level => Writing data in saves failed");
    }
}

// OTHER
std::string LevelLoader::getK(std::string content, std::string k) {
    size_t pos = content.find("<k>" + k + "</k>");
    if (pos != std::string::npos) {

        size_t tagPos = pos + 3 + k.length() + 4 + 1;
        char tag = content[tagPos];

        if (tag != 't') {
            size_t start = tagPos + 2;
            std::string tempContent = content.substr(start);
            size_t end = tempContent.find("</" + std::string(1, tag) + ">");

            return tempContent.substr(0, end);
        } else return "";
    } else return "";
}

bool LevelLoader::containsK(std::string content, std::string k) {
    size_t pos = content.find("<k>" + k + "</k>");
    if (pos != std::string::npos) {

        size_t tagPos = pos + 3 + k.length() + 4 + 1;
        char tag = content[tagPos];

        return (tag != 't');
    } else return false;
}

float LevelLoader::toNumber(std::string k) {
    if (k != "") return std::stoi(k);
    else return 0.0f;
}
