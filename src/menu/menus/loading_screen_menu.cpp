#include "menu/menus/loading_screen_menu.h"

#include "game.h"
#include "level/scenery.h"

// INIT
LoadingScreenMenu::LoadingScreenMenu(Game* g, MenuID i) : Menu(g, i) {
    initSyncLoad();
    initCamera();
    initScenery();
    initLoadingBar();
    initAsyncLoad();
}

void LoadingScreenMenu::initSyncLoad() {
    static H2DE_Engine* engine = game->getEngine();
    engine->loadAssetsSync("assets/preload", true);
}

void LoadingScreenMenu::initScenery() {
    scenery = new Scenery(game, 1, 1);
}

void LoadingScreenMenu::initCamera() {
    static H2DE_Engine* engine = game->getEngine();
    engine->getCamera()->setTranslate({ 0.0f, 6.0f }); // temp
}

void LoadingScreenMenu::initLoadingBar() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = { 0.0f, -2.0f }; // temp
    od.index = gameData->getIndex(LAYER_UI, 0);
    od.absolute = true;

    H2DE_BarObjectData bod = H2DE_BarObjectData();
    bod.min = 0.0f;
    bod.max = 100.0f;
    bod.value = 0.0f;

    loadingBar = engine->createObject<H2DE_BarObject>(od, bod);

    H2DE_SurfaceData borderSD = H2DE_SurfaceData();
    borderSD.transform.scale = { 7.0f, 0.534f }; // temp
    borderSD.index = 1;
    H2DE_TextureData borderTD = H2DE_TextureData();
    borderTD.textureName = "slidergroove-uhd.png";
    loadingBar->addSurfaceToBackground<H2DE_Texture>("border", borderSD, borderTD);

    H2DE_SurfaceData fillSD = H2DE_SurfaceData();
    fillSD.transform.scale = borderSD.transform.scale;
    borderSD.index = 0;
    H2DE_TextureData fillTD = H2DE_TextureData();
    fillTD.textureName = "sliderBar-uhd.png";
    loadingBar->addSurfaceToFill<H2DE_Texture>("fill", fillSD, fillTD);
}

void LoadingScreenMenu::initAsyncLoad() {
    static H2DE_Engine* engine = game->getEngine();

    engine->loadAssetsAsync("assets/load", [this](float blend) {
        loadingBar->setValue(blend * 100.0f);
        
    }, [this]() {
        static H2DE_Engine* engine = game->getEngine();

        engine->createDelay(500, [this]() {
            game->openMenu(MENU_ID_MAIN_MENU);
            delete game->loadingScreen;
            game->loadingScreen = nullptr;
        });
    }, true);
}

// CLEANUP
LoadingScreenMenu::~LoadingScreenMenu() {
    destroyScenery();
    destroyLoadingBar();
}

void LoadingScreenMenu::destroyScenery() {
    delete scenery;
}

void LoadingScreenMenu::destroyLoadingBar() {
    static H2DE_Engine* engine = game->getEngine();
    engine->destroyObject(loadingBar);
}

// UPDATE
void LoadingScreenMenu::update() {

}
