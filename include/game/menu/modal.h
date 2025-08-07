#pragma once

#include "game.h"
class Game;

class Modal {
public:
    Modal(Game* game, ModalID id);
    ~Modal();
    
    void close();

    constexpr const Data::ModalBuffer& getBuffer() const {
        return buffer;
    }

private:
    Game* game;
    ModalID id;
    Data::ModalBuffer buffer;

    std::vector<H2DE_Object*> objects = {};

    void initBuffer();
    void create();

    void destroyObjects();
};
