#ifndef MEGAHCK_H
#define MEGAHCK_H

class Game;
#include "game.h"

class Megahack {
private:
    Game* game;
    std::unordered_map<std::string, Hack*> hacks;

    std::vector<Hitbox*> hitboxTrail;

    void loadHacks();
    void saveHacks();

public:
    Megahack(Game* game);
    ~Megahack();

    void update();
    void render();
    void reset();
    void resetHitboxTrail();

    std::unordered_map<std::string, Hack*> getHacks() const;
    Hack* getHack(std::string hack);
};

#endif
