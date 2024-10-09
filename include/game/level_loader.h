#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <fstream>
#include <cstdlib>
#include <sstream>
#include "game.h"

class LevelLoader {
private:
    static void getMenuInfos(LevelData* data, json* level);
    static void getConfigInfos(LevelData* data, json* level);
    static void getBlocksInfos(LevelData* data, json* level, json* itemsData);
    static void getTriggersInfos(LevelData* data, json* level, json* itemsData);
    static void getStartposInfo(LevelData* data, json* level, json* itemsData);

    static void initLevelsInfos(std::string content, int id);
    static void initSaveInfos(std::string content, int id);

    static std::string getK(std::string content, std::string k);
    static bool containsK(std::string content, std::string k);
    static float toNumber(std::string k);

public:
    static LevelData* getLevelData(int id);
    static void loadLevelData(const fs::path& gmdFile);
};

#endif
