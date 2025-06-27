#ifndef SAVE_H
#define SAVE_H

#include <H2DE/engine/H2DE_json.h>
#include "utils.h"

class Save {
public:
    Save();
    ~Save() = default;

    inline const int getTransitionDuration() const {
        return static_cast<int>(data.at("megahack").at("transition-duration")) * 0.5f;
    }

    inline const PlayerIcons getPlayerIcons() const {
        return PlayerIcons{
            static_cast<Icon_ID>(data.at("player").at("icons").at("cube")),
            static_cast<Icon_ID>(data.at("player").at("icons").at("ship")),
            static_cast<Color_ID>(data.at("player").at("colors").at("col1")),
            static_cast<Color_ID>(data.at("player").at("colors").at("col2")),
            static_cast<bool>(data.at("player").at("glow")),
        };
    }

    void setPlayerCubeID(Icon_ID id);
    void setPlayerShipID(Icon_ID id);
    void setPlayerCol1(Color_ID id);
    void setPlayerCol2(Color_ID id);
    void setPlayerGlow(bool glow);

private:
    json data;

    std::filesystem::path filePath = "data/save.dat";

    void initFile();
    void initMegahack(json& fileData);
    void initPlayer(json& fileData);

    void loadData();
    void reload();
};

#endif
