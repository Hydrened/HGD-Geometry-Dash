#include "save.h"

// INIT
Save::Save() {
    initFile();
    loadData();
}

void Save::initFile() {
    if (std::filesystem::exists("save.dat")) {
        return;
    }

    json fileData;
    fileData["megahack"] = {};
    fileData["megahack"]["transition-duration"] = 500;

    H2DE_CreateJsonFile("save.dat", fileData, false, false);
}

void Save::loadData() {
    data = H2DE_ReadJsonFile("save.dat");
}

// CLEANUP
Save::~Save() {
    
}

// GETTER
const int Save::getTransitionDuration() const {
    return static_cast<int>(data["megahack"]["transition-duration"]) / 2;
}
