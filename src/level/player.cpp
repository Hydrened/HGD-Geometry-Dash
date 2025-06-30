#include "level/player.h"

#include "game.h"
#include "level/level.h"
#include "level/scenery.h"
#include "level/items/item.h"

// INIT
Player::Player(Game* g, Level* l, Scenery* s, const Checkpoint& c, const PlayerIcons& i) : game(g), level(l), scenery(s), checkpoint(c), icons(i) {
    createIcons();
    initState();
}

void Player::initState() {
    static const Data* gameData = game->getData();

    setGamemode(checkpoint.gamemode, 0);
    setSize(checkpoint.size);
    setGravity(checkpoint.gravity);
    setVelocityX(gameData->getSpeedVelocityX(checkpoint.speed));
}

void Player::createIcons() {
    normalCube = createIcon(PLAYER_GAMEMODE_CUBE, PLAYER_SIZE_NORMAL, icons.cubeID);
    miniCube = createIcon(PLAYER_GAMEMODE_CUBE, PLAYER_SIZE_MINI, icons.cubeID);
    normalShip = createIcon(PLAYER_GAMEMODE_SHIP, PLAYER_SIZE_NORMAL, icons.shipID);
    miniShip = createIcon(PLAYER_GAMEMODE_SHIP, PLAYER_SIZE_MINI, icons.shipID);
}

H2DE_BasicObject* Player::createIcon(PlayerGamemode gamemode, PlayerSize size, Icon_ID id) {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();
    static const float& miniPlayerRatio = gameData->getMiniPlayerRatio();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.translate = checkpoint.translate;
    od.index = Data::getIndex(LAYER_P, 0);

    H2DE_BasicObject* icon = engine->createObject<H2DE_BasicObject>(od);

    Player::applySurfacesOnIcon(icon, gameData->getIconSurfacesBuffer(gamemode, size, icons, false));
    applyHitboxesOnIcon(icon, gameData->getPlayerHitbox(gamemode, size));

    icon->hide();
    return icon;
}

void Player::applySurfacesOnIcon(H2DE_BasicObject* icon, const std::vector<Data::IconSurfaceBuffer>& iconSurfacesBuffer) {
    for (int i = 0; i < iconSurfacesBuffer.size(); i++) {
        const Data::IconSurfaceBuffer& surfaceBuffer = iconSurfacesBuffer.at(i);

        const std::string name = "surface-" + std::to_string(i);

        H2DE_SurfaceData sd = H2DE_SurfaceData();
        sd.transform.translate = surfaceBuffer.dest.getTranslate();
        sd.transform.scale = surfaceBuffer.dest.getScale();
        sd.transform.rotation = surfaceBuffer.rotation;
        sd.index = surfaceBuffer.index;

        H2DE_TextureData td = H2DE_TextureData();
        td.textureName = surfaceBuffer.textureName;
        td.color = surfaceBuffer.color;
        td.srcRect = surfaceBuffer.src;

        icon->addSurface<H2DE_Texture>(name, sd, td);
    }
}

void Player::applyHitboxesOnIcon(H2DE_BasicObject* icon, const PlayerHitbox& hitboxes) const {
    static const Data* gameData = game->getData();

    const std::array<std::pair<std::string, H2DE_LevelRect>, 2> hitboxeRects = {
        std::pair<std::string, H2DE_LevelRect>{ "red", hitboxes.red }, 
        { "blue", hitboxes.blue }
    };

    for (const auto& [color, hitboxRect] : hitboxeRects) {
        H2DE_Hitbox hitbox = H2DE_Hitbox();
        hitbox.transform.translate = hitboxRect.getTranslate();
        hitbox.transform.scale = hitboxRect.getScale();
        hitbox.color = gameData->getHitboxColor(color);

        icon->addHitbox(color, hitbox);
    }
}

// CLEANUP
Player::~Player() {
    destroyObjects();
}

void Player::destroyObjects() {
    static H2DE_Engine* engine = game->getEngine();
    
    engine->destroyObject(normalCube);
    engine->destroyObject(miniCube);
    engine->destroyObject(normalShip);
    engine->destroyObject(miniShip);
}

// UPDATE
void Player::update() {
    updateReset();
    updateGravity();
    updateSpecialBlocksCollision();
    updateClick();
    updateTranslationFromVelocity();
    updateGroundCollisions();
    updateItemCollisions();
    updateRotation();
}
 
void Player::updateReset() {
    onSolid = false;
}

void Player::updateGravity() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* gameData = game->getData();

    const float& g = gameData->getGravity(gamemode, size);
    const float& maxGravity = gameData->getMaxGravity(gamemode, size);

    velocity.y -= g * gravity;
    velocity.y = std::clamp(velocity.y, -maxGravity, maxGravity);
}

void Player::updateSpecialBlocksCollision() {

}

void Player::updateClick() {
    if (mouseDown && !dead) {
        click();
    }
}

void Player::updateTranslationFromVelocity() {
    H2DE_Translate translate = getTranslate();
    translate += velocity;
    setTranslate(translate);
}

void Player::updateGroundCollisions() {
    static const Data* gameData = game->getData();

    const H2DE_LevelRect& botGroundHitboxRect = scenery->getBotGround()->getHitboxWorldRect("main");
    const H2DE_LevelRect& topGroundHitboxRect = scenery->getTopGround()->getHitboxWorldRect("main");

    H2DE_LevelRect redHitboxRect = getCurrentRedHitboxWorldRect();

    if (redHitboxRect.collides(botGroundHitboxRect)) {
        const std::optional<H2DE_Face> possibleCollidedFace = redHitboxRect.getCollidedFace(botGroundHitboxRect);

        if (canSnap(possibleCollidedFace)) {
            snapHitboxRectToOtherHitboxRect(redHitboxRect, botGroundHitboxRect, possibleCollidedFace.value());
            velocity.y = 0.0f;
            onSolid = true;
        }

    } else if (redHitboxRect.collides(topGroundHitboxRect)) {
        const std::optional<H2DE_Face> possibleCollidedFace = redHitboxRect.getCollidedFace(topGroundHitboxRect);

        if (canSnap(possibleCollidedFace)) {
            snapHitboxRectToOtherHitboxRect(redHitboxRect, topGroundHitboxRect, possibleCollidedFace.value());
            velocity.y = 0.0f;
            onSolid = true;
        }
    }
}

void Player::updateItemCollisions() {
    if (level == nullptr) {
        return;
    }

    updateBlockCollisions();
    updateTriggerCollisions();
}

void Player::updateBlockCollisions() {
    if (dead) {
        return;
    }

    H2DE_LevelRect playerRedHitboxRect = getCurrentRedHitboxWorldRect();
    H2DE_LevelRect playerBlueHitboxRect = getCurrentBlueHitboxWorldRect();

    for (Block* block : level->getBlocks()) {
        if (dead) {
            return;
        }

        updateBlockCollisions(block, playerRedHitboxRect, playerBlueHitboxRect);
    }
}

void Player::updateBlockCollisions(Block* block, H2DE_LevelRect& playerRedHitboxRect, H2DE_LevelRect& playerBlueHitboxRect) {
    const BlockType& blockType = block->getType();
    if (blockType == BLOCK_TYPE_DECORATION) {
        return;
    }

    const H2DE_LevelRect& blockHitboxRect = block->getHitboxWorldRect();
    if (blockHitboxRect.getScale().isNull()) {
        return;
    }

    if (playerRedHitboxRect.collides(blockHitboxRect)) {
        
    }
}

void Player::updateTriggerCollisions() {
    if (dead) {
        return;
    }
}

// -- rotation
void Player::updateRotation() {
    if (dead) {
        return;
    }

    updateRotationOnSolid();
    updateRotationInAir();
    updateClampRotation();
    updateObjectRotation();
}

void Player::updateRotationOnSolid() {
    static const Data* gameData = game->getData();

    constexpr float RIGHT_ANGLE = 90.0f;
    constexpr float HALF_RIGHT_ANGLE = RIGHT_ANGLE * 0.5f;

    if (!onSolid) {
        return;
    }
    
    const float remain = (gravity == PLAYER_GRAVITY_NORMAL)
        ? RIGHT_ANGLE - fmod(H2DE::abs(rotation), RIGHT_ANGLE)
        : fmod(H2DE::abs(rotation), RIGHT_ANGLE);

    const float& rotationIncr = gameData->getRotation(PLAYER_GAMEMODE_CUBE, size);
    const int gravityMultiplier = gravity;

    if (remain != 0.0f) {
        if (remain < HALF_RIGHT_ANGLE) {
            rotation += ((remain > rotationIncr) ? rotationIncr : remain) * gravityMultiplier;
        } else {
            rotation -= ((RIGHT_ANGLE - remain > rotationIncr) ? rotationIncr : RIGHT_ANGLE - remain) * gravityMultiplier;
        }
    }
}

void Player::updateRotationInAir() {
    static const Data* gameData = game->getData();

    if (onSolid) {
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
            rotation = H2DE::pow(H2DE::abs(velocity.y / maxGravity), 1.15f) * (velocity.y / maxGravity < 0 ? -1 : 1) * gamemodeRotation; // temp => 1.15f : get from data
            break;

        default: break;
    }
}

void Player::updateClampRotation() {
    if (rotation > 360.0f) {
        rotation -= 360.0f;

    } else if (rotation < 0.0f) {
        rotation += 360.0f;
    }
}

void Player::updateObjectRotation() {
    getCurrentGamemodeObject()->setRotation(rotation);
}

// ACTIONS
void Player::click() {
    static const Data* gameData = game->getData();

    const float& click = gameData->getClick(gamemode, size);

    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE:
            if (isOnSolid()) {
                velocity.y = click * gravity;
            }
            break;

        case PLAYER_GAMEMODE_SHIP:
            break;

        default: return;
    }
} 

void Player::snapHitboxRectToOtherHitboxRect(const H2DE_LevelRect& hitboxRect, const H2DE_LevelRect& otherHitboxRect, H2DE_Face face) {
    H2DE_Translate res = getTranslate();

    switch (face) {
        case H2DE_FACE_TOP:
            res.y = otherHitboxRect.getMaxY() + hitboxRect.getScale().y * 0.5f;
            break;

        case H2DE_FACE_BOTTOM:
            res.y = otherHitboxRect.getMinY() - hitboxRect.getScale().y * 0.5f;
            break;

        case H2DE_FACE_RIGHT:
            break;

        case H2DE_FACE_LEFT:
            break;

        default: return;
    }

    setTranslate(res);
}

// GETTER
const Speed Player::getSpeed() const {
    static const Data* gameData = game->getData();
    return gameData->getSpeedFromVelocityX(velocity.x);
}

bool Player::canSnap(const std::optional<H2DE_Face>& possibleCollidedFace) const {
    static const Data* gameData = game->getData();

    if (!possibleCollidedFace.has_value()) {
        return false;
    }

    const H2DE_Face& collidedFace = possibleCollidedFace.value();
    const PlayerSnap& playerSnap = gameData->getPlayerSnap(gamemode, size);

    const H2DE_Face botFace = (gravity == PLAYER_GRAVITY_NORMAL) ? H2DE_FACE_TOP : H2DE_FACE_BOTTOM;
    const H2DE_Face topFace = (botFace == H2DE_FACE_BOTTOM) ? H2DE_FACE_BOTTOM : H2DE_FACE_TOP;

    bool isFalling = (gravity == PLAYER_GRAVITY_NORMAL) ? (velocity.y < 0.0f) : (velocity.y > 0.0f);

    bool canSnapOnBotFace = (playerSnap.bot && collidedFace == botFace && isFalling);
    bool canSnapOnTopFace = (playerSnap.top && collidedFace == topFace && !isFalling);

    return (canSnapOnBotFace || canSnapOnTopFace);
}

bool Player::isOnSolid() const {
    if (isOnGround()) {
        return true;
    }

    return isOnBlock();
}

bool Player::isOnGround() const {
    const H2DE_LevelRect& botGroundHitboxRect = scenery->getBotGround()->getHitboxWorldRect("main");
    const H2DE_LevelRect& topGroundHitboxRect = scenery->getTopGround()->getHitboxWorldRect("main");

    H2DE_LevelRect redHitboxRect = getCurrentRedHitboxWorldRect();
    redHitboxRect.y += (gravity == PLAYER_GRAVITY_NORMAL) ? -0.001f : 0.001f;

    if (redHitboxRect.collides(botGroundHitboxRect)) {
        if (canSnap(redHitboxRect.getCollidedFace(botGroundHitboxRect))) {
            return true;
        }
    }

    if (redHitboxRect.collides(topGroundHitboxRect)) {
        if (canSnap(redHitboxRect.getCollidedFace(topGroundHitboxRect))) {
            return true;
        }
    }

    return false;
}

bool Player::isOnBlock() const {
    H2DE_LevelRect redHitboxRect = getCurrentRedHitboxWorldRect();
    redHitboxRect.y += (gravity == PLAYER_GRAVITY_NORMAL) ? -0.001f : 0.001f;

    // for (const Item* item : level->getItems()) {

    // }

    return false;
}

H2DE_BasicObject* Player::getCurrentGamemodeObject() const {
    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE: switch (size) {
            case PLAYER_SIZE_NORMAL: return normalCube;
            case PLAYER_SIZE_MINI: return miniCube;
            default: return nullptr;
        } break;

        case PLAYER_GAMEMODE_SHIP: switch (size) {
            case PLAYER_SIZE_NORMAL: return normalShip;
            case PLAYER_SIZE_MINI: return miniShip;
            default: return nullptr;
        } break;

        default: return nullptr;
    }
}

H2DE_LevelRect Player::getCurrentHitboxWorldRect(const std::string& color) const {
    return getCurrentGamemodeObject()->getHitboxWorldRect(color);
}

// SETTER
void Player::setTranslate(const H2DE_Translate& translate) {
    for (H2DE_BasicObject* object : getObjects()) {
        object->setTranslate(translate);
    }
}

void Player::setGamemode(PlayerGamemode g, int yEntry) {
    getCurrentGamemodeObject()->hide();
    gamemode = g;
    getCurrentGamemodeObject()->show();
}

void Player::setSize(PlayerSize s) {
    getCurrentGamemodeObject()->hide();
    size = s;
    getCurrentGamemodeObject()->show();
}

void Player::setGravity(PlayerGravity g) {
    gravity = g;

    for (H2DE_BasicObject* object : getObjects()) {
        const H2DE_Scale& scale = object->getScale();
        object->setScale({ scale.x, H2DE::abs(scale.y) * g});
    }
}
