#include "level_loader.h"

// MAIN
LevelData* LevelLoader::getLevelData(int id) {
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
            throw std::runtime_error("HGD-1004: Error loading level data => " + std::string(e.what()));
        }
    } else throw std::runtime_error("HGD-1005: Error loading level data => Level ID not found");

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
            id = -1; // not uploaded level (give special id)
        } else id = std::stoi(getK(content, "k1"));

        initLevelsInfos(content, id);
        initSaveInfos(content, id);

        std::cout << "Level data loaded in " << SDL_GetTicks() - start << "ms" << std::endl;
    } else throw std::runtime_error("HGD-2002: Error loading extern level => File isn't a gmd file");
}

// GET DATA
void LevelLoader::getMenuInfos(LevelData* data, json* level) {
    json menuInfos = (*level)["menuInfos"];
    json meColor = menuInfos["backgroundColor"];

    data->name = menuInfos["name"];
    data->difficulty = menuInfos["difficulty"];
    data->menuColor = { static_cast<Uint8>(meColor["r"]), static_cast<Uint8>(meColor["g"]), static_cast<Uint8>(meColor["b"]), 255 };
}

void LevelLoader::getConfigInfos(LevelData* data, json* level)  {
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

void LevelLoader::getBlocksInfos(LevelData* data, json* level, json* itemsData)  {
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
                bBlock->hitboxOffset = { 1.0f - (float)(hitSize["w"]) - (float)(hitOffset["x"]), 1.0f - (float)(hitSize["h"]) - (float)(hitOffset["y"]) };
                break;
            case 270:
                bBlock->hitboxSize = { hitSize["h"], hitSize["w"] };
                bBlock->hitboxOffset = { 1.0f - (float)(hitSize["h"]) - (float)(hitOffset["y"]), 1.0f - (float)(hitSize["w"]) - (float)(hitOffset["x"]) };
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
        if (itemData.contains("special")) {
            bBlock->specialData = ItemSpecialData();
            bBlock->specialData.type = itemData["special"]["type"];
            bBlock->specialData.desc = itemData["special"]["desc"];
        }

        if (itemData.contains("sprites")) bBlock->sprites = itemData["sprites"];

        data->blocks.push_back(bBlock);
    }
}

void LevelLoader::getTriggersInfos(LevelData* data, json* level, json* itemsData) {
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

void LevelLoader::getStartposInfo(LevelData* data, json* level, json* itemsData) {
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

// INIT DATA
void LevelLoader::initLevelsInfos(std::string content, int id) {
    std::string levelsPATH = "data/levels.json";
    json* levels = H2DE_Json::read(levelsPATH);
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

    (*levels)[std::to_string(id)] = level;
    if (!H2DE_Json::write(levelsPATH, levels)) {
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
    if (!H2DE_Json::write(savesPATH, saves)) {
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
