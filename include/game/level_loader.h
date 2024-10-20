#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <fstream>
#include <cstdlib>
#include <sstream>
#include "game.h"

class LevelLoader {
private:
    static void getMenuInfos(Game* game, LevelData* data, json* level);
    static void getConfigInfos(Game* game, LevelData* data, json* level);
    static void getBlocksInfos(Game* game, LevelData* data, json* level);
    static void getTriggersInfos(Game* game, LevelData* data, json* level);
    static void getStartposInfo(Game* game, LevelData* data, json* level);

    static void initLevelsInfos(std::string content, int id);
    static void initSaveInfos(std::string content, int id);

    static std::string getK(std::string content, std::string k);
    static bool containsK(std::string content, std::string k);
    static float toNumber(std::string k);

public:
    static LevelData* getLevelData(Game* game, int id);
    static void loadLevelData(const fs::path& gmdFile);
};

#endif
