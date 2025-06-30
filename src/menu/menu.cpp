#include "menu/menu.h"

#include "game.h"
#include "menu/transition.h"
#include "menu/modal.h"

// INIT
Menu::Menu(Game* g, MenuID i) : game(g), id(i) {
    Transition* transition = new Transition(game, TRANSITION_STATE_IN, nullptr);
    initSong();
    initObjects();
}

void Menu::setCameraTranslate(const H2DE_Translate& translate) {
    static H2DE_Engine* engine = game->getEngine();
    engine->getCamera()->setTranslate(translate);
}

void Menu::initSong() {
    static H2DE_Audio* audio = game->getEngine()->getAudio();

    if (!audio->isSongPlaying()) {
        audio->playSong("menu_loop.mp3", -1, false);
    }
}

// -- objects
void Menu::initObjects() {
    static const Data* gameData = game->getData();

    for (const Data::MenuObjectBuffer& objectBuffer : gameData->getMenuObjects(id)) {
        createObject(objectBuffer);
    }
}

void Menu::createObject(const Data::MenuObjectBuffer& objectBuffer) {
    H2DE_Object* object = nullptr;

    switch (objectBuffer.type) {
        case Data::OBJECT_BUFFER_TYPE_BASIC:
            object = createBasicObject(objectBuffer);
            break;

        case Data::OBJECT_BUFFER_TYPE_BUTTON: {
            object = createButtonObject(objectBuffer);
            break;
        }

        case Data::OBJECT_BUFFER_TYPE_TEXT:
            object = createTextObject(objectBuffer);
            break;

        default: return;
    }

    if (object == nullptr) {
        return;
    }

    for (int i = 0; i < objectBuffer.hitboxes.size(); i++) {
        object->addHitbox("hitbox_" + std::to_string(i), objectBuffer.hitboxes.at(i));
    }

    objects.push_back(object);
}

H2DE_BasicObject* Menu::createBasicObject(const Data::MenuObjectBuffer& objectBuffer) const {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_BasicObject* basic = engine->createObject<H2DE_BasicObject>(objectBuffer.objectData);

    for (int i = 0; i < objectBuffer.surfaces.size(); i++) {
        const Data::SurfaceBuffer& surfaceBuffer = objectBuffer.surfaces.at(i);
        basic->addSurface<H2DE_Texture>("surface-" + std::to_string(i), surfaceBuffer.surface, surfaceBuffer.texture);
    }

    return basic;
}

H2DE_ButtonObject* Menu::createButtonObject(const Data::MenuObjectBuffer& objectBuffer) const {
    static H2DE_Engine* engine = game->getEngine();

    if (!objectBuffer.button.has_value()) {
        return nullptr;
    }

    H2DE_ButtonObject* button = engine->createObject<H2DE_ButtonObject>(objectBuffer.objectData, objectBuffer.button.value());

    for (int i = 0; i < objectBuffer.surfaces.size(); i++) {
        const Data::SurfaceBuffer& surfaceBuffer = objectBuffer.surfaces.at(i);
        button->addSurface<H2DE_Texture>("surface-" + std::to_string(i), surfaceBuffer.surface, surfaceBuffer.texture);
    }

    return button;
}

H2DE_TextObject* Menu::createTextObject(const Data::MenuObjectBuffer& objectBuffer) const {
    static H2DE_Engine* engine = game->getEngine();

    if (!objectBuffer.text.has_value()) {
        return nullptr;
    }

    return engine->createObject<H2DE_TextObject>(objectBuffer.objectData, objectBuffer.text.value());
}

// CLEANUP
Menu::~Menu() {
    destroyObjects();
    destroyModal();
}

void Menu::destroyObjects() {
    static H2DE_Engine* engine = game->getEngine();

    for (H2DE_Object* object : objects) {
        engine->destroyObject(object);
    }
}

void Menu::destroyModal() {
    if (modal != nullptr) {
        delete modal;
        modal = nullptr;
    }
}

// ACTIONS
void Menu::close(const std::function<void()>& callback) {
    disableButtons();

    Transition* transition = new Transition(game, TRANSITION_STATE_OUT, [this, callback]() {
        delete this;

        if (callback) {
            callback();
        }
    });
}

void Menu::disableButtons() {
    for (H2DE_Object* object : objects) {
        H2DE_ButtonObject* button = dynamic_cast<H2DE_ButtonObject*>(object);

        if (button == nullptr) {
            continue;
        }

        button->disable();
    }
}

void Menu::openModal(ModalID id) {
    if (!isAModalOpened()) {
        modal = new Modal(game, id);
    }
}

void Menu::closeModal() {
    if (isAModalOpened()) {
        modal->close();
        modal = nullptr;
    }
}
