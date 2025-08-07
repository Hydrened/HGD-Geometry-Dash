#include "save.h"

// INIT
Save::Save() {
    initFile();
    loadData();
}

void Save::initFile() {
    if (std::filesystem::exists(filePath)) {
        return;
    }

    json fileData;
    
    initMegahack(fileData);
    initPlayer(fileData);

    H2DE_Json::create(filePath, fileData, false, false);
}

void Save::initMegahack(json& fileData) {
    fileData["megahack"] = {};
    fileData["megahack"]["transition-duration"] = 500;
    fileData["megahack"]["noclip"] = false;
    fileData["megahack"]["speedhack"] = false;
    fileData["megahack"]["speedhack-speed"] = 1.0f;
    fileData["megahack"]["show-hitboxes"] = false;
    fileData["megahack"]["hitboxes-trail"] = false;
}

void Save::initPlayer(json& fileData) {
    fileData["player"] = {};

    fileData["player"]["icons"] = {};
    fileData["player"]["icons"]["cube"] = 1;
    fileData["player"]["icons"]["ship"] = 1;

    fileData["player"]["colors"] = {};
    fileData["player"]["colors"]["col1"] = 0;
    fileData["player"]["colors"]["col2"] = 3;
    fileData["player"]["glow"] = false;
}

// LOAD
void Save::loadData() {
    data = H2DE_Json::read(filePath);
}

void Save::reload() {
    H2DE_Json::write(filePath, data, false);
    loadData();
}

// SETTER
void Save::setPlayerCubeID(IconID id) {
    data["player"]["icons"]["cube"] = static_cast<int>(id);
    reload();
}

void Save::setPlayerShipID(IconID id) {
    data["player"]["icons"]["ship"] = static_cast<int>(id);
    reload();
}

void Save::setPlayerCol1(ColorID id) {
    data["player"]["colors"]["col1"] = static_cast<int>(id);
    reload();
}

void Save::setPlayerCol2(ColorID id) {
    data["player"]["colors"]["col2"] = static_cast<int>(id);
    reload();
}

void Save::setPlayerGlow(bool glow) {
    data["player"]["glow"] = glow;
    reload();
}
