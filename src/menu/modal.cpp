#include "menu/modal.h"

// INIT
Modal::Modal(Game* g, ModalID i) : game(g), id(i) {
    initBuffer();
    create();
}

void Modal::initBuffer() {
    static const Data* gameData = game->getData();
    buffer = gameData->getModalBuffer(id);
}

void Modal::create() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.absolute = true;
    od.index = Data::getIndex(LAYER_UI, 1000);

    H2DE_BasicObject* background = engine->createObject<H2DE_BasicObject>(od);
    gameData->createStretchedTextureFromSquare(background, "square01_001-uhd.png", { 5.0f, 3.0f });

    objects.push_back(background);
}

// CLEANUP
Modal::~Modal() {
    destroyObjects();
}

void Modal::destroyObjects() {
    static H2DE_Engine* engine = game->getEngine();

    for (H2DE_Object* object : objects) {
        engine->destroyObject(object);
    }
}

// ACTIONS
void Modal::close() {
    delete this;
}
