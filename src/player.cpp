#include "player.h"

// INIT
Player::Player(Game* g, const Checkpoint* c) : game(g), checkpoint(c) {
    initCheckpoint();
    initIcons();
}

void Player::initCheckpoint() {
    if (checkpoint == nullptr) {
        return;
    }

    pos = checkpoint->pos;
    velocity = checkpoint->velocity;
    rotation = checkpoint->rotation;

    gamemode = checkpoint->gamemode;
    size = checkpoint->size;
    gravity = checkpoint->gravity;
}

void Player::initIcons() {
    initCube();
}

void Player::initIconCreateLayers(const std::string& textureName, const std::vector<std::tuple<std::string, H2DE_AbsRect, H2DE_LevelRect, H2DE_ColorRGB>>& layers, std::unordered_map<std::string, H2DE_Surface*>& surfaces) const {
    static H2DE_Engine* engine = game->getEngine();

    for (const auto& [name, src, dest, color] : layers) {

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.textureName = textureName;
        sd.rect = dest;
        sd.color = color;

        H2DE_TextureData td = H2DE_TextureData();
        td.srcRect = src;

        surfaces[name] = H2DE_CreateTexture(engine, sd, td);
    }
}

void Player::initCube() {
    static H2DE_Engine* engine = game->getEngine();

    int cubeID = 1; // temp
    const IconData& cubeData = game->getData()->getCube(cubeID);
    const std::string textureName = "player_" + formatID(cubeID, 2) + "-uhd.png";

    static const std::pair<H2DE_LevelRect, H2DE_LevelRect>& bigCubeHitboxes = game->getData()->getPlayerHitbox(gamemode, size);

    H2DE_Hitbox blueSnapHitbox = H2DE_Hitbox();
    blueSnapHitbox.rect = bigCubeHitboxes.first;
    blueSnapHitbox.color = { 255, 127, 127, 255 };
    blueSnapHitbox.collisionIndex = 2;
    blueSnapHitbox.snap = true;
    blueSnapHitbox.onCollide = [this](H2DE_Object* object, H2DE_Face face) {
        bool normalGravityAndBotOnSolid = (gravity == PLAYER_GRAVITY_NORMAL && face == H2DE_FACE_BOTTOM);
        bool upsideDownGravityAndTopOnSolid = (gravity == PLAYER_GRAVITY_UPSIDE_DOWN && face == H2DE_FACE_TOP);

        botOnSolid = normalGravityAndBotOnSolid || upsideDownGravityAndTopOnSolid;

        bool normalGravityAndTopOnSolid = (gravity == PLAYER_GRAVITY_NORMAL && face == H2DE_FACE_TOP);
        bool upsideDownGravityAndBotOnSolid = (gravity == PLAYER_GRAVITY_UPSIDE_DOWN && face == H2DE_FACE_BOTTOM);

        topOnSolid = normalGravityAndTopOnSolid || upsideDownGravityAndBotOnSolid;
    };

    H2DE_ObjectData od = H2DE_ObjectData();
    od.pos = game->convertToLevelPos(pos, bigCubeHitboxes.first.getSize());
    od.size = bigCubeHitboxes.first.getSize();
    od.pivot = od.size.getCenter();
    od.index = getIndex(LAYER_P, 0);
    od.hitboxes["blue-snap"] = blueSnapHitbox;

    const std::vector<std::tuple<std::string, H2DE_AbsRect, H2DE_LevelRect, H2DE_ColorRGB>> layers = {
        { "glow", cubeData.glowSrcRect, cubeData.glowDestRect, { 255, 255, 255, 0 } },
        { "col-1", cubeData.col1SrcRect, cubeData.col1DestRect, { 125, 255, 0, 255 } },
        { "col-2", cubeData.col2SrcRect, cubeData.col2DestRect, { 0, 255, 255, 255 } },
    };

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    initIconCreateLayers(textureName, layers, bod.surfaces);

    cube = H2DE_CreateBasicObject(engine, od, bod);
}

// CLEANUP
Player::~Player() {
    destroyIcons();
}

void Player::destroyIcons() {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_DestroyObject(engine, cube);
}

// UPDATE
void Player::update() {
    updateGravity();
    updateClicks();
    updateRotation();
    updatePosition();
}

void Player::updateGravity() {
    static const Data* data = game->getData();

    const float& gravity = data->getGravity(gamemode, size);
    const float& maxGravity = data->getMaxGravity(gamemode, size);

    pos = game->convertToGdPos(H2DE_GetObjectPos(cube), { 1.0f, 1.0f }); // temp

    velocity.y += gravity;

    if (velocity.y > maxGravity) {
        velocity.y = maxGravity;

    } else if (velocity.y < -maxGravity) {
        velocity.y = -maxGravity;
    }
}

void Player::updateClicks() {
    if (mouseDown) {
        click();
    }
}

void Player::updateRotation() {
    updateRotationOnSolid();
    updateRotationInAir();
    updateRotationClamp();
    H2DE_SetObjectRotation(cube, rotation);
}

void Player::updateRotationOnSolid() {
    static const Data* gameData = game->getData();

    bool onSolid = (botOnSolid || topOnSolid);
    if (!onSolid) {
        return;
    }

    const float& rotationIncr = gameData->getRotation(PLAYER_GAMEMODE_CUBE, size);

    const float remain = (gravity == PLAYER_GRAVITY_NORMAL)
        ? 90.0f - fmod(std::abs(rotation), 90.0f)
        : fmod(std::abs(rotation), 90.0f);
    const int gravityMultiplier = gravity;

    if (remain != 0.0f) {
        if (remain < 45.0f) {
            rotation += ((remain > rotationIncr) ? rotationIncr : remain) * gravityMultiplier;

        } else {
            rotation -= ((90.0f - remain > rotationIncr) ? rotationIncr : 90.0f - remain) * gravityMultiplier;
        }
    }
}

void Player::updateRotationInAir() {
    static const Data* gameData = game->getData();

    bool inAir = (!botOnSolid && !topOnSolid);
    if (!inAir) {
        return;
    }

    const float& maxGravity = gameData->getMaxGravity(gamemode, size);
    const float& gamemodeRotation = gameData->getRotation(gamemode, size);
    const int gravityMultiplier = gravity;

    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE:
            rotation += gamemodeRotation * gravityMultiplier;
            break;

        case PLAYER_GAMEMODE_SHIP:
            rotation = pow(abs(velocity.y / maxGravity), 1.15f) * (velocity.y / maxGravity < 0 ? -1 : 1) * gamemodeRotation;
            break;

        default: break;
    }
}

void Player::updateRotationClamp() {
    if (rotation > 360.0f) {
        rotation -= 360.0f;

    } else if (rotation < 0.0f) {
        rotation += 360.0f;
    }
}

void Player::updatePosition() {
    pos += velocity;
    H2DE_SetObjectPos(cube, game->convertToLevelPos(pos, { 1.0f, 1.0f })); // temp
}

// EVENTS
void Player::kill() {

}

void Player::click() {
    static const Data* gameData = game->getData();
    const float& click = gameData->getClick(gamemode, size);

    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE:
            if (!botOnSolid) {
                break;
            }

            velocity.y = click;
            botOnSolid = false;
            break;

        case PLAYER_GAMEMODE_SHIP:
            
            break;

        default: break;
    }
}

// GETTER
const H2DE_LevelPos Player::getPos() const {
    return pos;
}

// SETTER 
void Player::setVelocityX(float vx) {
    velocity.x = vx;
}

void Player::setMouseDown(bool state) {
    mouseDown = state;
}
