#include "menu.h"

// INIT
Menu::Menu(Game* g, MenuID i) : game(g), id(i) {
    if (id == MAIN_MENU) {
        initMainMenu();
    }

    createObjects();
    playSong();
}

void Menu::initMainMenu() {
    static H2DE_Engine* engine = game->getEngine();
    static const H2DE_LevelPos& defaultCameraPos = game->getData()->getDefaultCameraPos();
    const H2DE_ColorRGB& defaultGroundColor = game->getData()->getDefaultGroundColor();
    const H2DE_ColorRGB& defaultBackgroundColor = game->getData()->getDefaultBackgroundColor();

    H2DE_SetCameraPos(engine, game->convertToCamPos(defaultCameraPos));

    scenery = new Scenery(game, 1, 1);

    mainMenuColorLoop = H2DE_CreateTimeline(engine, 30000, H2DE_EASING_LINEAR, [this, defaultGroundColor, defaultBackgroundColor](float blend) {

        H2DE_ColorRGB interpolatedGroundColor = defaultGroundColor;
        interpolatedGroundColor = interpolatedGroundColor.addHue(H2DE_Lerp(0.0f, 360.0f, blend, H2DE_EASING_LINEAR));
        scenery->setGroundColor(interpolatedGroundColor);

        H2DE_ColorRGB interpolatedBackgroundColor = defaultBackgroundColor;
        interpolatedBackgroundColor = interpolatedBackgroundColor.addHue(H2DE_Lerp(0.0f, 360.0f, blend, H2DE_EASING_LINEAR));
        scenery->setBackgroundColor(interpolatedBackgroundColor);

    }, nullptr, -1, true);
}

void Menu::createObjects() {
    for (const ObjectBuffer& buffer : game->getData()->getMenuObjects(id)) {
        switch (buffer.type) {
            case BAR_OBJECT: createBarObject(buffer); break;
            case BASIC_OBJECT: createBasicObject(buffer); break;
            case BUTTON_OBJECT: createButtonObject(buffer); break;
            case TEXT_OBJECT: createTextObject(buffer); break;
            default: continue;
        }
    }
}

void Menu::createBarObject(const ObjectBuffer& buffer) {
    static H2DE_Engine* engine = game->getEngine();
    BarObjectBuffer barBuffer = buffer.barBuffer.value();

    if (!buffer.barBuffer.has_value()) {
        return;
    }

    createObjectInitBOD(buffer, barBuffer.fsdb, barBuffer.bod.front);
    createObjectInitBOD(buffer, barBuffer.bsdb, barBuffer.bod.front);

    H2DE_BarObject* bar = H2DE_CreateBarObject(engine, buffer.od, barBuffer.bod);
    objects.push_back(bar);
}

void Menu::createBasicObject(const ObjectBuffer& buffer) {
    static H2DE_Engine* engine = game->getEngine();
    BasicObjectBuffer basicBuffer = buffer.basicBuffer.value();

    if (!buffer.basicBuffer.has_value()) {
        return;
    }

    createObjectInitBOD(buffer, basicBuffer.sdb, basicBuffer.bod.surfaces);

    H2DE_BasicObject* basic = H2DE_CreateBasicObject(engine, buffer.od, basicBuffer.bod);
    objects.push_back(basic);
}

void Menu::createButtonObject(const ObjectBuffer& buffer) {
    static H2DE_Engine* engine = game->getEngine();
    ButtonObjectBuffer buttonBuffer = buffer.buttonBuffer.value();

    if (!buffer.buttonBuffer.has_value()) {
        return;
    }

    createObjectInitBOD(buffer, buttonBuffer.sdb, buttonBuffer.bod.surfaces);

    H2DE_ButtonObject* button = H2DE_CreateButtonObject(engine, buffer.od, buttonBuffer.bod);
    objects.push_back(button);
}

void Menu::createTextObject(const ObjectBuffer& buffer) {
    static H2DE_Engine* engine = game->getEngine();
    TextObjectBuffer textBuffer = buffer.textBuffer.value();

    if (!buffer.textBuffer.has_value()) {
        return;
    }

    H2DE_TextObject* text = H2DE_CreateTextObject(engine, buffer.od, textBuffer.tod);
    objects.push_back(text);
}

void Menu::createObjectInitBOD(const ObjectBuffer& buffer, const std::unordered_map<std::string, SurfaceDataBuffer>& surfaceDataBuffers, std::unordered_map<std::string, H2DE_Surface*>& surfaces) {
    static H2DE_Engine* engine = game->getEngine();

    for (const auto& [name, surfaceBuffer] : surfaceDataBuffers) {

        H2DE_Surface* surface = nullptr;

        if (surfaceBuffer.texD.has_value()) {
            surface = H2DE_CreateTexture(engine, surfaceBuffer.surD, surfaceBuffer.texD.value());
        }
        else if (surfaceBuffer.sprD.has_value()) {
            surface = H2DE_CreateSprite(engine, surfaceBuffer.surD, surfaceBuffer.sprD.value());
        }
        else continue;
        
        surfaces["main"] = surface;
    }
}

void Menu::playSong() const {
    static H2DE_Engine* engine = game->getEngine();

    if (H2DE_IsSongPlaying(engine)) {
        return;
    }

    H2DE_PlaySong(engine, "menu_loop.mp3", -1, true);
}

// CLEANUP
Menu::~Menu() {
    game->destroyObjects(objects);

    if (scenery) {
        destroyScenery();
    }
    if (mainMenuColorLoop != -1) {
        destroyMainMenuColorLoop();
    }
}

void Menu::destroyScenery() {
    delete scenery;
}

void Menu::destroyMainMenuColorLoop() {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_StopTimeline(engine, mainMenuColorLoop, false);
}

// RDM
void Menu::disableButtons() const {
    for (H2DE_Object* object : objects) {
        H2DE_ButtonObject* button = dynamic_cast<H2DE_ButtonObject*>(object);

        if (button) {
            H2DE_SetButtonOnMouseDown(button, nullptr);
            H2DE_SetButtonOnMouseUp(button, nullptr);
            H2DE_SetButtonOnHover(button, nullptr);
            H2DE_SetButtonOnBlur(button, nullptr);
        }
    }
}

// UPDATE
void Menu::update() {
    if (!scenery) {
        return;
    }

    game->updateCamera(1);
    scenery->update();
}
