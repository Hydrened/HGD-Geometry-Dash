#include "menu.h"

// INIT
Menu::Menu(Game* g) : game(g) {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Camera* camera = game->getCamera();

    camera->setPos(gameData->positions->camera, 0);
    backgroundPos = gameData->positions->background;
    backgroundColor = gameData->colors->menuBackground;
    groundColor = gameData->colors->menuGround;

    H2DE_Timeline* iconLoop = H2DE_CreateTimeline(engine, 3000, LINEAR, NULL, [this]() {
        spawnIcon();
    }, -1);
    H2DE_AddTimelineToManager(tm, iconLoop);

    Color defaultBackgroundColor = backgroundColor;
    Color defaultGroundColor = groundColor;
    H2DE_Timeline* colorLoop = H2DE_CreateTimeline(engine, 30000, LINEAR, [this, defaultBackgroundColor, defaultGroundColor](float blend) {

        H2DE_RGB rgbBG = H2DE_AddHueToRGB((H2DE_RGB)(defaultBackgroundColor), lerp(0, 360, blend));
        backgroundColor.r = rgbBG.r;
        backgroundColor.g = rgbBG.g;
        backgroundColor.b = rgbBG.b;

        H2DE_RGB rgbG = H2DE_AddHueToRGB((H2DE_RGB)(defaultGroundColor), lerp(0, 360, blend) - 25);
        groundColor.r = rgbG.r;
        groundColor.g = rgbG.g;
        groundColor.b = rgbG.b;
        
    }, NULL, -1);
    H2DE_AddTimelineToManager(tm, colorLoop);

    spawnIcon();
    H2DE_PlaySound(engine, 0, "menu_loop.mp3", -1);
}

// CLEANUP
Menu::~Menu() {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_DestroyTimelineManager(tm);
}

void Menu::resetMainMenu() {
    H2DE_ResetTimelineManager(tm);
    Game::delay(100, [this]() {
        spawnIcon();
    });
}

// UPDATE
void Menu::update() {
    static GameData* gameData = game->getData();
    Camera* camera = game->getCamera();

    LevelPos camPos = camera->getPos();
    GameState state = game->getState();

    switch (state.main) {
        case MAIN_MENU:
            backgroundPos.x += (gameData->physics->speeds[1] * gameData->physics->backgroundRatio);
            camera->setPos({ camPos.x + gameData->physics->speeds[1], camPos.y }, 0);

            H2DE_TickTimelineManager(tm);
            updateIcon();
            break;

        default: icon = nullptr; break;
    }
}

void Menu::updateIcon() {
    static int FPS = H2DE_GetEngineFPS(game->getEngine());
    static GameData* gameData = game->getData();
    static float groundHeight = gameData->sizes->ground.h;
    static float bottomGroundsTop = gameData->positions->botGround.y + groundHeight;
    static std::random_device rd;

    if (icon) {
        Gamemode gamemode = icon->gamemode;
        Size size = icon->size;

        icon->velocity.x = gameData->physics->speeds[icon->speed];
        icon->velocity.y += gameData->physics->gravities[gamemode][size];
        icon->pos.x += icon->velocity.x;
        icon->pos.y += icon->velocity.y;

        float maxGravity = gameData->physics->maxGravities[gamemode][size];
        if (icon->velocity.y > maxGravity) icon->velocity.y = maxGravity;
        else if (icon->velocity.y * -1.0f > maxGravity) icon->velocity.y = maxGravity * -1.0f;

        if (icon->velocity.y < 0.0f) {
            if (icon->pos.y < bottomGroundsTop) {
                icon->onSolid = true;
                icon->velocity.y = 0.0f;
                icon->pos.y = bottomGroundsTop;
            }
        }

        if (icon->onSolid) {
            float defaultRotationIncr = gameData->physics->rotations[CUBE][size];
            int remain = 90 - fmod(std::abs(icon->rotation), 90);

            if (remain != 0) {
                if (remain < 45) icon->rotation += ((remain > defaultRotationIncr) ? defaultRotationIncr : remain);
                else icon->rotation -= ((90 - remain > defaultRotationIncr) ? defaultRotationIncr : 90 - remain);
            }
        } else {
            float gmRotation = gameData->physics->rotations[gamemode][size];
            
            switch (gamemode) {
                case CUBE: icon->rotation += gmRotation; break;
                case SHIP: icon->rotation = pow(abs(icon->velocity.y / maxGravity), 1.15f) * (icon->velocity.y / maxGravity < 0 ? -1 : 1) * gmRotation; break;
                default: break;
            }
        }

        if (icon->rotation > 360) icon->rotation -= 360;
        else if (icon->rotation < -360) icon->rotation += 360;

        std::mt19937 gen(rd());
        std::uniform_int_distribution<> rdmJump(0, FPS);
        std::uniform_int_distribution<> rdmHoldDuration(50, 300);

        if (!icon->holding) {
            if (rdmJump(gen) == 0) switch (gamemode) {
                case CUBE: if (icon->onSolid) {
                    icon->onSolid = false;
                    icon->velocity.y = gameData->physics->clicks[CUBE][size];
                } break;
                case SHIP: if (!icon->holding) {
                    icon->onSolid = false;
                    icon->holding = true;
                    Game::delay(rdmHoldDuration(gen), [this]() {
                        if (icon) icon->holding = false;
                    });
                    icon->velocity.y += (gameData->physics->clicks[SHIP][size]);
                } break;
                default: break;
            }
        } else {
            switch (gamemode) {
                case SHIP: icon->velocity.y += (gameData->physics->clicks[SHIP][size]); break;
                default: break;
            }
        }
    }
}

// RENDER
void Menu::render() {
    GameState state = game->getState();

    switch (state.main) {
        case MAIN_MENU: renderMainMenu(); renderIcon(); break;
        case LEVEL_MENU: renderLevelMenu(); break;
        default: break;
    }
}

void Menu::renderMainMenu() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    // Level elements
    H2DE_GraphicObject* ground = H2DE_CreateGraphicObject();
    ground->type = IMAGE;
    ground->texture = "ground_1.png"; // replace => ground_1.png(last ground used)
    ground->pos = calculator->convertToPx(gameData->positions->botGround, gameData->sizes->ground, false, false);
    ground->size = calculator->convertToPx(gameData->sizes->ground);
    ground->rgb = (H2DE_RGB)(groundColor);
    ground->repeatX = true;
    ground->index = Zindex{G, 0}.getIndex();
    H2DE_AddGraphicObject(engine, ground);

    H2DE_GraphicObject* line = H2DE_CreateGraphicObject();
    line->type = IMAGE;
    line->texture = "line_1.png";
    line->pos = calculator->convertToPx({ gameData->offsets->botLine.x, gameData->positions->botGround.y + gameData->offsets->botLine.y }, gameData->sizes->line, true, false);
    line->size = calculator->convertToPx(gameData->sizes->line);
    line->rgb = { 255, 255, 255, 255 };
    line->index = Zindex{G, 1}.getIndex();
    H2DE_AddGraphicObject(engine, line);

    H2DE_GraphicObject* background = H2DE_CreateGraphicObject(*ground);
    background->texture = "background_1.png"; // replace => background_1.png(last background used)
    background->pos = calculator->convertToPx(backgroundPos, gameData->sizes->background, false, false);
    background->size = calculator->convertToPx(gameData->sizes->background);
    background->rgb = (H2DE_RGB)(backgroundColor);
    background->index = Zindex{BG, 0}.getIndex();
    H2DE_AddGraphicObject(engine, background);



    // Menu elements
    H2DE_GraphicObject* gameTitle = H2DE_CreateGraphicObject();
    gameTitle->type = IMAGE;
    gameTitle->texture = "game-title.png";
    gameTitle->pos = calculator->convertToPxAbs(gameData->positions->gameTitle);
    gameTitle->size = calculator->convertToPxAbs(gameData->sizes->gameTitle);
    gameTitle->index = Zindex{UI, 0}.getIndex();
    H2DE_AddGraphicObject(engine, gameTitle);

    H2DE_GraphicObject* levelMenuButton = H2DE_CreateGraphicObject(*gameTitle);
    levelMenuButton->texture = "level-menu-button.png";
    levelMenuButton->pos = calculator->convertToPxAbs(gameData->positions->levelMenuButton);
    levelMenuButton->size = calculator->convertToPxAbs(gameData->sizes->levelMenuButton);
    levelMenuButton->onclick = [this]() {
        this->game->setState({ LEVEL_MENU, DEFAULT }, game->getTransitionDuration(500), NULL);
    };
    H2DE_AddGraphicObject(engine, levelMenuButton);

    H2DE_GraphicObject* shopMenuButton = H2DE_CreateGraphicObject(*gameTitle);
    shopMenuButton->texture = "shop-menu-button.png";
    shopMenuButton->pos = calculator->convertToPxAbs(gameData->positions->shopMenuButton);
    shopMenuButton->size = calculator->convertToPxAbs(gameData->sizes->shopMenuButton);
    H2DE_AddGraphicObject(engine, shopMenuButton);

    H2DE_GraphicObject* onlineMenuButton = H2DE_CreateGraphicObject(*gameTitle);
    onlineMenuButton->texture = "online-menu-button.png";
    onlineMenuButton->pos = calculator->convertToPxAbs(gameData->positions->onlineMenuButton);
    onlineMenuButton->size = calculator->convertToPxAbs(gameData->sizes->onlineMenuButton);
    onlineMenuButton->onclick = [this]() {
        throw std::runtime_error("Abuse pas");
    };
    H2DE_AddGraphicObject(engine, onlineMenuButton);

    H2DE_GraphicObject* closeGameButton = H2DE_CreateGraphicObject(*gameTitle);
    closeGameButton->texture = "close-game-button.png";
    closeGameButton->pos = calculator->convertToPxAbs(gameData->positions->closeGameButton);
    closeGameButton->size = calculator->convertToPxAbs(gameData->sizes->closeGameButton);
    closeGameButton->onclick = [this]() {
        game->openModal({ MAIN_MENU, MODAL_EXIT });
    };
    H2DE_AddGraphicObject(engine, closeGameButton);
}

void Menu::renderLevelMenu() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    H2DE_GraphicObject* ground = H2DE_CreateGraphicObject();
    ground->type = IMAGE;
    ground->texture = "ground_1.png"; // replace => ground_1.png(last ground used)
    ground->pos = calculator->convertToPx(gameData->positions->botGround, gameData->sizes->ground, false, false);
    ground->size = calculator->convertToPx(gameData->sizes->ground);
    ground->rgb = { 0, 102, 255, 255 }; // replace => rgb effect
    ground->repeatX = true;
    ground->index = Zindex{G, 0}.getIndex();
    H2DE_AddGraphicObject(engine, ground);
}

void Menu::renderIcon() {
    static GameData* gameData = game->getData();

    if (icon) {
        renderMainTexture();
        if (gameData->other->iconSecondTexture[icon->gamemode]) renderSecondTexture();
    }
}

void Menu::renderMainTexture() {
    static H2DE_Engine* engine = game->getEngine();
    static GameData* gameData = game->getData();
    static Calculator* calculator = game->getCalculator();

    Gamemode gamemode = icon->gamemode;
    Size size = icon->size;

    LevelSize iconSize = gameData->sizes->mainIcon[gamemode][size];
    LevelOffset iconOffset = gameData->offsets->mainIcon[gamemode][size];
    LevelPos iconPos = { icon->pos.x + iconOffset.x, icon->pos.y + iconOffset.y };
    std::string gamemodeStrigified = gameData->other->gamemodeStringified[gamemode];
    H2DE_Pos center = calculator->convertToPx(LevelOffset{ -iconOffset.x + gameData->sizes->redHitbox[gamemode][size].w / 2, iconOffset.y + gameData->sizes->redHitbox[gamemode][size].h / 2 });

    H2DE_GraphicObject* col1 = H2DE_CreateGraphicObject();
    col1->type = IMAGE;
    col1->texture = gamemodeStrigified + "-" + std::to_string(icon->id) + "-1.png";
    col1->pos = calculator->convertToPx(iconPos, iconSize, true, false);
    col1->size = calculator->convertToPx(iconSize);
    col1->rgb = (H2DE_RGB)(gameData->colors->icons[icon->col1]);
    col1->rotationOrigin = center;
    col1->rotation = icon->rotation;
    col1->index = Zindex{ T1, 2 }.getIndex();
    H2DE_AddGraphicObject(engine, col1);

    H2DE_GraphicObject* col2 = H2DE_CreateGraphicObject(*col1);
    col2->texture = gamemodeStrigified + "-" + std::to_string(icon->id) + "-2.png";
    col2->rgb = (H2DE_RGB)(gameData->colors->icons[icon->col2]);
    col2->index = Zindex{ T1, 1 }.getIndex();
    H2DE_AddGraphicObject(engine, col2);
}

void Menu::renderSecondTexture() {
    static Calculator* calculator = game->getCalculator();
    static GameData* gameData = game->getData();
    static H2DE_Engine* engine = game->getEngine();

    Gamemode gamemode = icon->gamemode;
    Size size = icon->size;

    LevelSize iconSize = { gameData->sizes->secondIcon[gamemode][size].w, gameData->sizes->secondIcon[gamemode][size].h };
    LevelOffset iconOffset = gameData->offsets->secondIcon[gamemode][size];
    LevelPos iconPos = { icon->pos.x + iconOffset.x, icon->pos.y + iconOffset.y };
    H2DE_Pos center = calculator->convertToPx(LevelOffset{ -iconOffset.x + gameData->sizes->redHitbox[gamemode][size].w / 2, iconSize.h + iconOffset.y - gameData->sizes->redHitbox[gamemode][size].h / 2 });

    H2DE_GraphicObject* col1 = H2DE_CreateGraphicObject();
    col1->type = IMAGE;
    col1->texture = "cube-0-1.png";
    col1->pos = calculator->convertToPx(iconPos, iconSize, true, false);
    col1->size = calculator->convertToPx(iconSize);
    col1->rotation = icon->rotation;
    col1->rotationOrigin = center;
    col1->rgb = (H2DE_RGB)(gameData->colors->icons[icon->col1]);
    col1->index = Zindex{ T1, -2 }.getIndex();
    H2DE_AddGraphicObject(engine, col1);

    H2DE_GraphicObject* col2 = H2DE_CreateGraphicObject(*col1);
    col2->texture = "cube-0-2.png";
    col2->rgb = (H2DE_RGB)(gameData->colors->icons[icon->col2]);
    col2->index = Zindex{ T1, -1 }.getIndex();
    H2DE_AddGraphicObject(engine, col2);
}

// GETTER
int Menu::getLevelIndex() const {
    return levelIndex;
}

// SETTER
void Menu::incrLevelIndex(int incr) {
    int max = 1;

    if (levelIndex + incr < 0) levelIndex = max;
    else if (levelIndex + incr > max) levelIndex = 0;
    else levelIndex += incr;
}

// OTHER
void Menu::spawnIcon() {
    static GameData* gameData = game->getData();
    static std::random_device rd;
    static std::vector<Gamemode> gamemodes = { CUBE, SHIP };

    std::mt19937 gen(rd());

    std::uniform_int_distribution<> rdmCubes(1, 13);
    std::uniform_int_distribution<> rdmShips(1, 1);
    std::uniform_int_distribution<> rdmGamemodes(0, 1);
    std::uniform_int_distribution<> rdmSizes(1, 1);
    std::uniform_int_distribution<> rdmSpeeds(1, 1);
    std::uniform_int_distribution<> rdmCol(0, gameData->colors->icons.size() - 1);
    std::uniform_int_distribution<> rdmGlow(0, 0);

    int speed = rdmSpeeds(gen);
    int size = rdmSizes(gen);
    int glow = rdmGlow(gen);
    Gamemode gamemode = gamemodes[rdmGamemodes(gen)];
    int textureId = (gamemode == CUBE) ? rdmCubes(gen): rdmShips(gen);

    icon = new MenuIcon();
    icon->id = textureId;
    icon->pos = { -2.0f, 0 };
    icon->velocity = { gameData->physics->speeds[speed], 0.0f };
    icon->gamemode = gamemode;
    icon->size = (size == 0) ? MINI : BIG;
    icon->speed = speed;
    icon->col1 = rdmCol(gen);
    icon->col2 = rdmCol(gen);
    icon->glow = (glow == 0) ? false : true;
    icon->rotation = 0;
    icon->onSolid = true;
    icon->holding = false;
}
