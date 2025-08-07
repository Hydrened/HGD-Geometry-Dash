#pragma once

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
            static_cast<IconID>(data.at("player").at("icons").at("cube")),
            static_cast<IconID>(data.at("player").at("icons").at("ship")),
            static_cast<ColorID>(data.at("player").at("colors").at("col1")),
            static_cast<ColorID>(data.at("player").at("colors").at("col2")),
            static_cast<bool>(data.at("player").at("glow")),
        };
    }

    void setPlayerCubeID(IconID id);
    void setPlayerShipID(IconID id);
    void setPlayerCol1(ColorID id);
    void setPlayerCol2(ColorID id);
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
