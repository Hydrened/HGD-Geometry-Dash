#include "menu/menus/main_menu.h"

#include "level/scenery.h"
#include "level/player.h"

// INIT
MainMenu::MainMenu(Game* g, MenuID i) : Menu(g, i) {
    initCamera();
    initScenery();
    initColorLoop();
    initPlayerLoop();
}

void MainMenu::initCamera() {
    static const H2DE_Translate& defaultCameraTranslate = game->getData()->getDefaultCameraTranslate();
    setCameraTranslate(defaultCameraTranslate);
}

void MainMenu::initScenery() {
    scenery = new Scenery(game, 1, 1);
}

void MainMenu::initColorLoop() {
    static H2DE_Engine* engine = game->getEngine();

    const H2DE_ColorRGB& defaultGroundColor = game->getData()->getDefaultGroundColor();
    const H2DE_ColorRGB& defaultBackgroundColor = game->getData()->getDefaultBackgroundColor();

    colorLoop = engine->createTimeline(30000, H2DE_EASING_LINEAR, [this, defaultGroundColor, defaultBackgroundColor](float blend) {

        H2DE_ColorRGB interpolatedGroundColor = defaultGroundColor;
        interpolatedGroundColor = interpolatedGroundColor.addHue(H2DE::lerp(0.0f, 360.0f, blend, H2DE_EASING_LINEAR));
        scenery->setGroundColor(interpolatedGroundColor);

        H2DE_ColorRGB interpolatedBackgroundColor = defaultBackgroundColor;
        interpolatedBackgroundColor = interpolatedBackgroundColor.addHue(H2DE::lerp(0.0f, 360.0f, blend, H2DE_EASING_LINEAR));
        scenery->setBackgroundColor(interpolatedBackgroundColor);

    }, nullptr, -1, false);
}

void MainMenu::initPlayerLoop() {
    randomSpawnDelay();
}

// CLEANUP
MainMenu::~MainMenu() {
    destroyScenery();
    destroyPlayer();
    destroyColorLoop();
    destroyPlayerLoop();
}

void MainMenu::destroyScenery() {
    delete scenery;
}

void MainMenu::destroyPlayer() {
    if (player != nullptr) {
        delete player;
    }
}

void MainMenu::destroyColorLoop() {
    static H2DE_Engine* engine = game->getEngine();
    engine->stopTimeline(colorLoop, false);
}

void MainMenu::destroyPlayerLoop() {
    static H2DE_Engine* engine = game->getEngine();
    engine->stopDelay(playerLoop, false);
}

// UPDATE
void MainMenu::update() {
    updateCamera();
    updateScenery();
    updatePlayer();
}

void MainMenu::updateCamera() {
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    static const float& speed = game->getData()->getSpeedVelocityX(1);

    H2DE_Translate cameraTranslate = camera->getTranslate();
    cameraTranslate.x += speed;

    camera->setTranslate(cameraTranslate);
}

void MainMenu::updateScenery() {
    scenery->update();
}

void MainMenu::updatePlayer() {
    if (player != nullptr) {
        player->update();
    }
}

// ACTIONS
void MainMenu::randomSpawnDelay() {
    static H2DE_Engine* engine = game->getEngine();

    spawnPlayer();

    // temp => 2300: temps pour la speed -1 de passer entierement (actuellment pareil maos pour la speed 1)
    playerLoop = engine->delay(H2DE::randomIntegerInRange(2300, 5000), [this]() {
        randomSpawnDelay();
    }, false);
}

void MainMenu::spawnPlayer() {
    static const Data* gameData = game->getData();
    static H2DE_Camera* camera = game->getEngine()->getCamera();
    
    destroyPlayer();

    const auto [speed, gamemode, size] = gameData->getRandomPlayerState();

    Checkpoint checkpoint = Checkpoint();
    checkpoint.translate = { camera->getWorldRect().getMinX() - 2.0f, 0.0f };
    checkpoint.speed = speed;
    checkpoint.gamemode = gamemode;
    checkpoint.size = size;

    PlayerIcons icons = gameData->getRandomPlayerIcons();

    player = new Player(game, nullptr, scenery, checkpoint, icons);
    player->setVelocityX(gameData->getSpeedVelocityX(1) + gameData->getSpeedVelocityX(speed));
}

void MainMenu::click(const H2DE_Translate& translate) {
    if (player == nullptr) {
        return;
    }

    if (player->getCurrentRedHitboxWorldRect().collides(translate)) {
        player->kill();
        delete player;
        player = nullptr;
    }
}
