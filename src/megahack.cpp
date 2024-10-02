#include "megahack.h"

// INIT
Megahack::Megahack(Game* g) : game(g) {
    loadHacks();
}

void Megahack::loadHacks() {
    json* data = H2DE_Json::read("data/saves.json");
    json hs = (*data)["hacks"];

    for (int i = 0; i < hs.size(); i++) {
        json h = hs[i];

        Hack* hack = new Hack();
        hack->active = h["active"];
        hack->keycode = h["keycode"];

        hacks[h["name"]] = hack;
    }
}

// CLEANUP
Megahack::~Megahack() {
    saveHacks();

    for (const auto& hack : hacks) delete hack.second;
    hacks.clear();
}

void Megahack::saveHacks() {
    std::string SAVESpath = "data/saves.json";
    json* saves = H2DE_Json::read(SAVESpath);

    for (const auto& hack : hacks) {
        for (int i = 0; i < (*saves)["hacks"].size(); i++) {
            if ((*saves)["hacks"][i]["name"] == hack.first) {
                (*saves)["hacks"][i]["active"] = hack.second->active;
            }
        }
    }

    if (!H2DE_Json::write(SAVESpath, saves)) {
        throw std::runtime_error("HGD-3003: Error saving hacks => Writing hacks failed");
    }
}   

// GETTER
std::unordered_map<std::string, Hack*> Megahack::getHacks() const {
    return hacks;
}

Hack* Megahack::getHack(std::string hack) {
    return hacks[hack];
}
