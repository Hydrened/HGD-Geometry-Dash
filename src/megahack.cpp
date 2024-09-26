#include "megahack.h"

// INIT
Megahack::Megahack(Game* g) : game(g) {
    loadData();
}

void Megahack::loadData() {
    json* data = H2DE_Json::read("data/saves.json");

    std::cout << (*data)["hacks"] << std::endl;
}

// CLEANUP
Megahack::~Megahack() {
    
}
