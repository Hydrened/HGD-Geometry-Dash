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
    reset();

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

void Megahack::reset() {
    resetHitboxTrail();
}

void Megahack::resetHitboxTrail() {
    for (Hitbox* hitbox : hitboxTrail) delete hitbox;
    hitboxTrail.clear();
}

// UPDATE
void Megahack::update() {
    static GameData* gameData = game->getData();
    Level* level = game->getLevel();
    LevelPos camPos = game->getCamera()->getPos();
    
    if (level) {
        if (hacks["hitbox-trail"]->active && hacks["show-hitboxes"]->active) {
            Player* player = game->getLevel()->getPlayer();

            Hitbox* hitbox = new Hitbox();
            hitbox->pos = player->getPos();
            hitbox->gamemode = player->getGamemode();
            hitbox->size = player->getSize();
            hitboxTrail.push_back(hitbox);

            for (int i = 0; i < hitboxTrail.size(); i++) {
                Hitbox* hitbox = hitboxTrail[i];

                if (hitbox->pos.x + gameData->sizes->redHitboxSizes[hitbox->gamemode][hitbox->size].w < camPos.x) {
                    delete hitboxTrail[i];
                    hitboxTrail.erase(hitboxTrail.begin() + i);
                } else break;
            }
        } else if (!hitboxTrail.empty()) resetHitboxTrail();
    }
}

// RENDER
void Megahack::render() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();
    Level* level = game->getLevel();

    if (level) {
        if (hacks["show-hitboxes"]->active) for (Hitbox* hitbox : hitboxTrail) {
            LevelPos pos = hitbox->pos;
            Gamemode gamemode = hitbox->gamemode;
            Size size = hitbox->size;

            H2DE_Size absRedHitboxSize = calculator->convertToPx(gameData->sizes->redHitboxSizes[gamemode][size]);
            LevelPos offsetRedHitboxPos = { pos.x + gameData->offsets->redHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->redHitboxOffsets[gamemode][size].y };
            
            H2DE_GraphicObject* redHitbox = new H2DE_GraphicObject();
            redHitbox->type = POLYGON;
            redHitbox->pos = calculator->convertToPx(offsetRedHitboxPos, gameData->sizes->iconSizes[gamemode][size], false, false);
            redHitbox->points = {
                { 0, 0 },
                { absRedHitboxSize.w, 0 },
                { absRedHitboxSize.w, absRedHitboxSize.h },
                { 0, absRedHitboxSize.h },
            };
            redHitbox->color = static_cast<H2DE_Color>(gameData->colors->hitboxes[OBSTACLE]);
            redHitbox->index = Zindex{ H, 0 }.getIndex();
            H2DE_AddGraphicObject(engine, redHitbox);

            LevelPos offsetBlueHitboxPos = { pos.x + gameData->offsets->blueHitboxOffsets[gamemode][size].x, pos.y + gameData->offsets->blueHitboxOffsets[gamemode][size].y };
            H2DE_Size blueHitboxSize = calculator->convertToPx(gameData->sizes->blueHitboxSizes[gamemode][size]);

            H2DE_GraphicObject* blueHitbox = new H2DE_GraphicObject(*redHitbox);
            blueHitbox->pos = calculator->convertToPx(offsetBlueHitboxPos, gameData->sizes->iconSizes[gamemode][size], false, false);
            blueHitbox->points = {
                { 0, 0 },
                { blueHitboxSize.w, 0 },
                { blueHitboxSize.w, blueHitboxSize.h },
                { 0, blueHitboxSize.h },
            };
            blueHitbox->color = static_cast<H2DE_Color>(gameData->colors->hitboxes[SOLID]);
            blueHitbox->index = Zindex{ H, 1 }.getIndex();
            H2DE_AddGraphicObject(engine, blueHitbox);
        }
    }
}

// GETTER
std::unordered_map<std::string, Hack*> Megahack::getHacks() const {
    return hacks;
}

Hack* Megahack::getHack(std::string hack) {
    return hacks[hack];
}
