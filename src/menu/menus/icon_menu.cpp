#include "menu/menus/icon_menu.h"

// INIT
IconMenu::IconMenu(Game* g, MenuID i) : Menu(g, i) {
    initCamera();
    loadPlayerIcons();
    createButtons();
    
    refreshMainIcon();
    selectLoadedPlayerIcons();
}

void IconMenu::initCamera() {
    static const H2DE_Translate& iconMenuCameraTranslate = game->getData()->getDefaultCameraTranslate();
    setCameraTranslate(iconMenuCameraTranslate);
}

void IconMenu::loadPlayerIcons() {
    static Save* save = game->getSave();
    icons = save->getPlayerIcons();
}

void IconMenu::createButtons() {
    createIconButtons();
    createColorButtons();
}

// -- button
H2DE_ButtonObject* IconMenu::createButton(std::vector<H2DE_ButtonObject*>& buttons, int index, bool unlocked, const H2DE_Translate& firstButtonTranslate, const H2DE_Translate& buttonScale, const std::function<void(int, bool)>& onMouseUp) {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();


    static const int& iconMenuButtonsOrder = gameData->getIconMenuButtonsOrder();
    static const float& iconMenuButtonSpacing = gameData->getIconMenuButtonSpacing();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = firstButtonTranslate + H2DE_Translate{ static_cast<float>(index), 0.0f } + H2DE_Translate{ index * iconMenuButtonSpacing, 0.0f };
    od.transform.scale = buttonScale;
    od.index = Data::getIndex(LAYER_UI, iconMenuButtonsOrder);
    od.absolute = true;

    H2DE_ButtonObjectData bod = gameData->getDefaultButtonObjectData([this, onMouseUp, index, unlocked]() {
        if (onMouseUp) {
            onMouseUp(index, unlocked);
        }
    });
    
    H2DE_ButtonObject* button = engine->createObject<H2DE_ButtonObject>(od, bod);
    buttons.push_back(button);

    return button;
}

void IconMenu::applyHitboxToButton(H2DE_ButtonObject* button) {
    static const Data* gameData = game->getData();

    H2DE_Hitbox hitbox = H2DE_Hitbox();
    hitbox.transform.scale = { 1.0f, 1.0f };
    hitbox.color = gameData->getHitboxColor("red");

    button->addHitbox("main", hitbox);
}

// -- -- icons
void IconMenu::createIconButtons() {
    static const Data* gameData = game->getData();

    static const H2DE_Translate& iconMenuFirstIconButtonTranslate = gameData->getIconMenuFirstIconButtonTranslate();
    static const H2DE_Scale& iconMenuIconButtonScale = gameData->getIconMenuIconButtonScale();
    static const uint8_t& nbIcons = gameData->getNbIcons(PLAYER_GAMEMODE_CUBE);

    for (int i = 0; i < nbIcons; i++) {
        bool unlocked = true;

        if (i > 3) unlocked = false; // temp

        H2DE_ButtonObject* iconButton = createButton(iconButtons, i, unlocked, iconMenuFirstIconButtonTranslate, iconMenuIconButtonScale, [this](int index, bool unlocked) {
            if (unlocked) {
                icons.cubeID = static_cast<Icon_ID>(index + 1);
                refreshMainIcon();
                selectButton(iconButtons, index);

            } else {
                openModal(MODAL_ID_QUIT_GAME);
            }
        });

        createIconButtons_applyTextures(iconButton, i + 1, unlocked);
        applyHitboxToButton(iconButton);
    }
}

void IconMenu::createIconButtons_applyTextures(H2DE_ButtonObject* icon, int index, bool unlocked) {
    static const Data* gameData = game->getData();

    if (unlocked) {
        PlayerIcons playerIcons = PlayerIcons();
        playerIcons.cubeID = static_cast<Icon_ID>(index);

        for (const Data::IconSurfaceBuffer& iconSurfaceBuffer : gameData->getIconSurfacesBuffer(PLAYER_GAMEMODE_CUBE, PLAYER_SIZE_NORMAL, playerIcons, true)) {
            H2DE_SurfaceData sd = H2DE_SurfaceData();
            sd.transform.translate = iconSurfaceBuffer.dest.getTranslate();
            sd.transform.scale = iconSurfaceBuffer.dest.getScale();
            sd.transform.rotation = iconSurfaceBuffer.rotation;

            H2DE_TextureData td = H2DE_TextureData();
            td.textureName = iconSurfaceBuffer.textureName;
            td.color = iconSurfaceBuffer.color;
            td.srcRect = iconSurfaceBuffer.src;

            icon->addSurface<H2DE_Texture>(std::to_string(iconSurfaceBuffer.index), sd, td);
        }

    } else {
        H2DE_TextureData td = H2DE_TextureData();
        td.textureName = "locked-icon.png";

        icon->addSurface<H2DE_Texture>("main", H2DE_SurfaceData(), td);
    }
}

// -- -- colors
void IconMenu::createColorButtons() {
    static const Data* gameData = game->getData();

    static const H2DE_Translate& iconMenuFirstCol1ButtonTranslate = gameData->getIconMenuFirstCol1ButtonTranslate();
    static const H2DE_Translate& iconMenuFirstCol2ButtonTranslate = gameData->getIconMenuFirstCol2ButtonTranslate();
    static const H2DE_Scale& iconMenuColorButtonScale = gameData->getIconMenuColorButtonScale();
    static const uint8_t& nbIconColors = gameData->getNbIconColors();

    for (int i = 0; i < nbIconColors; i++) {
        bool unlocked = true;

        if (i > 3) unlocked = false; // temp

        H2DE_ButtonObject* col1Button = createButton(col1Buttons, i, unlocked, iconMenuFirstCol1ButtonTranslate, iconMenuColorButtonScale, [this](int index, bool unlocked) {
            if (unlocked) {
                icons.col1 = static_cast<Color_ID>(index);
                refreshMainIcon();
                selectButton(col1Buttons, index);

            } else {
                openModal(MODAL_ID_QUIT_GAME);
            }
        });

        createColorButtons_applyTextures(col1Button, i, unlocked);
        applyHitboxToButton(col1Button);



        H2DE_ButtonObject* col2Button = createButton(col2Buttons, i, unlocked, iconMenuFirstCol2ButtonTranslate, iconMenuColorButtonScale, [this](int index, bool unlocked) {
            if (unlocked) {
                icons.col2 = static_cast<Color_ID>(index);
                refreshMainIcon();
                selectButton(col2Buttons, index);

            } else {
                openModal(MODAL_ID_QUIT_GAME);
            }
        });

        createColorButtons_applyTextures(col2Button, i, unlocked);
        applyHitboxToButton(col2Button);
    }
}

void IconMenu::createColorButtons_applyTextures(H2DE_ButtonObject* button, int index, bool unlocked) {
    static const Data* gameData = game->getData();

    H2DE_ColorData cd = H2DE_ColorData();
    cd.color = gameData->getIconColor(static_cast<Color_ID>(index));
    button->addSurface<H2DE_Color>("color", H2DE_SurfaceData(), cd);

    if (!unlocked) {
        const auto& [sd, td] = gameData->getSurfaceBuffer("gray-locker");
        button->addSurface<H2DE_Texture>("locker", sd, td);
    }
}

// CLEANUP
IconMenu::~IconMenu() {
    destroyMainIcon();
    destroyButtons();
    saveIcons();
}

void IconMenu::destroyMainIcon() {
    static H2DE_Engine* engine = game->getEngine();

    if (mainIcon != nullptr) {
        engine->destroyObject(mainIcon);
    }
}

void IconMenu::destroyButtons() {
    static H2DE_Engine* engine = game->getEngine();

    for (H2DE_ButtonObject* iconButton : iconButtons) {
        engine->destroyObject(iconButton);
    }

    for (H2DE_ButtonObject* col1Button : col1Buttons) {
        engine->destroyObject(col1Button);
    }

    for (H2DE_ButtonObject* col2Button : col2Buttons) {
        engine->destroyObject(col2Button);
    }
}

void IconMenu::saveIcons() {
    static Save* save = game->getSave();
    const PlayerIcons unmodifiedPlayerIcons = save->getPlayerIcons();

    if (unmodifiedPlayerIcons.cubeID != icons.cubeID) {
        save->setPlayerCubeID(icons.cubeID);
    }

    if (unmodifiedPlayerIcons.shipID != icons.shipID) {
        save->setPlayerShipID(icons.shipID);
    }

    if (unmodifiedPlayerIcons.col1 != icons.col1) {
        save->setPlayerCol1(icons.col1);
    }

    if (unmodifiedPlayerIcons.col2 != icons.col2) {
        save->setPlayerCol2(icons.col2);
    }

    if (unmodifiedPlayerIcons.glow != icons.glow) {
        save->setPlayerGlow(icons.glow);
    }
}

// UPDATE
void IconMenu::update() {

}

// ACTIONS
void IconMenu::refreshMainIcon() {
    destroyMainIcon();

    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();
    static const float& scaleMultiplier = gameData->getIconMenuMainIconScaleMultiplier();
    static const H2DE_Translate& translate = gameData->getIconMenuMainIconTranslate();
    static const int& getIconMenuMainIconOrder = gameData->getIconMenuMainIconOrder();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = translate;
    od.index = Data::getIndex(LAYER_UI, getIconMenuMainIconOrder);
    od.absolute = true;

    mainIcon = engine->createObject<H2DE_BasicObject>(od);

    for (const Data::IconSurfaceBuffer& surfaceBuffer : gameData->getIconSurfacesBuffer(PLAYER_GAMEMODE_CUBE, PLAYER_SIZE_NORMAL, icons, false)) {

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.transform.translate = surfaceBuffer.dest.getTranslate() * scaleMultiplier;
        sd.transform.scale = surfaceBuffer.dest.getScale() * scaleMultiplier;
        sd.transform.rotation = surfaceBuffer.rotation;
        sd.index = surfaceBuffer.index;

        H2DE_TextureData td = H2DE_TextureData();
        td.textureName = surfaceBuffer.textureName;
        td.srcRect = surfaceBuffer.src;
        td.color = surfaceBuffer.color;

        mainIcon->addSurface<H2DE_Texture>(std::to_string(surfaceBuffer.index), sd, td);
    }
}

void IconMenu::selectButton(const std::vector<H2DE_ButtonObject*>& buttons, int index) const {
    static const Data* gameData = game->getData();

    for (int i = 0; i < buttons.size(); i++) {
        H2DE_ButtonObject* button = buttons.at(i);

        if (button->hasSurface("selected")) {
            button->removeSurface("selected");
        }
        
        if (i == index) {
            const auto& [sd, td] = gameData->getSurfaceBuffer("icon-menu-selected-button");
            button->addSurface<H2DE_Texture>("selected", sd, td);
        }
    }
}

void IconMenu::selectLoadedPlayerIcons() {
    selectButton(iconButtons, static_cast<int>(icons.cubeID - 1));
    selectButton(col1Buttons, static_cast<int>(icons.col1));
    selectButton(col2Buttons, static_cast<int>(icons.col2));
}
