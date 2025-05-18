#include "player.h"

#undef min

// INIT
Player::Player(Game* g, Level* l, const Checkpoint& c) : game(g), level(l), checkpoint(c) {
    initCheckpoint();
    initIcons();
}

void Player::initCheckpoint() {
    respawn(checkpoint);
}

void Player::initIcons() {
    initCube();
    initShip();
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
    static const Data* gameData = game->getData();

    int cubeID = 1; // temp 
    const IconData& cubeData = gameData->getCube(cubeID);
    const std::string textureName = "player_" + formatID(cubeID, 2) + "-uhd.png";

    static const PlayerHitbox& bigCubeHitboxes = gameData->getPlayerHitbox(gamemode, size);

    H2DE_Hitbox redHitbox = H2DE_Hitbox();
    redHitbox.rect = bigCubeHitboxes.red;
    redHitbox.color = gameData->getHitboxColor("red");

    H2DE_Hitbox blueHitbox = H2DE_Hitbox();
    blueHitbox.rect = bigCubeHitboxes.blue;
    blueHitbox.color = gameData->getHitboxColor("blue");

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = pos.makeRect({ 1.0f, 1.0f });
    od.pivot = od.rect.getSize().getCenter();
    od.index = getIndex(LAYER_P, 0);
    od.hitboxes["red"] = redHitbox;
    od.hitboxes["blue"] = blueHitbox;

    const std::vector<std::tuple<std::string, H2DE_AbsRect, H2DE_LevelRect, H2DE_ColorRGB>> layers = {
        { "glow", cubeData.glowSrcRect, cubeData.glowDestRect, { 255, 255, 255, 0 } },
        { "col-1", cubeData.col1SrcRect, cubeData.col1DestRect, { 125, 255, 0, 255 } },
        { "col-2", cubeData.col2SrcRect, cubeData.col2DestRect, { 0, 255, 255, 255 } },
    };

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    initIconCreateLayers(textureName, layers, bod.surfaces);

    cube = H2DE_CreateBasicObject(engine, od, bod);
}

void Player::initShip() {

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
    onSolid = false;

    updateVelocity();
    updatePosition();
    updateRotation();
}

// velocity
void Player::updateVelocity() {
    /**
     * - 1: CALCUL DE LA VELOCITY :
     *          but: calcul de la velocity finale pour ensuite faire les calcul en fonction de la position modifiée par la velocity
     *          inclus : gravity, special blocks collisions, click
     */

    updateGravity();
    updateSpecialBlocksCollision();
    updateClick();

    pos += velocity;
    H2DE_SetObjectPos(getCurrentGamemodeObject(), pos);
}

void Player::updateGravity() {
    static const Data* gameData = game->getData();
    const float& g = gameData->getGravity(gamemode, size);
    const float& maxGravity = gameData->getMaxGravity(gamemode, size);

    velocity.y += g * gravity;
    velocity.y = std::clamp(velocity.y, -maxGravity, maxGravity);
}

void Player::updateSpecialBlocksCollision() {
    // exemple: les pads peuvent changer la velocity, donc il faut qu'il soit avant les recaluls de positions
    // avant le jump pour que les pads prennent le dessus sur les jumps
    // avant le jump aussi pour que quand je rentre dans un portail de ship, les clicks deviennent des clicks de ship et pas de cube
}

void Player::updateClick() {
    if (mouseDown && !dead) {
        click();
    }
}

// positions
void Player::updatePosition() {
    /** 
     * - 2: RECALCUL DES POSITIONS :
     *          but: une fois qu'on a la velocity de fin de frame en soit, on peut faire tout les calculs de positions
     *          note: peut reset la velocity.y à 0.0f
     *          inclus : ground collision, block collision
     */

    updateGroundsCollisions();
    updateItemsCollisions();
    H2DE_SetObjectPos(getCurrentGamemodeObject(), pos);
}

void Player::updateGroundsCollisions() {
    static const Data* gameData = game->getData();

    const H2DE_LevelRect botGroundRect = H2DE_GetObjectRect(level->getBotGround());
    const H2DE_LevelRect topGroundRect = H2DE_GetObjectRect(level->getTopGround());

    H2DE_LevelRect redHitboxRect = getCurrentRedHitboxWorldRect();
    const H2DE_LevelPos playerRedHitboxOffset = getCurrentRedHitboxOffset();
    const PlayerSnap& playerSnap = gameData->getPlayerSnap(gamemode, size);

    if (redHitboxRect.collides(botGroundRect)) {

        const std::optional<H2DE_Face> possibleCollidedFace = redHitboxRect.getCollidedFace(botGroundRect);

        if (canSnap(possibleCollidedFace)) {
            snapPlayerHitboxToItemHitbox(playerRedHitboxOffset, redHitboxRect, botGroundRect, possibleCollidedFace.value());
            velocity.y = 0.0f;
            onSolid = true;
        }

    } else if (redHitboxRect.collides(topGroundRect)) {
        
        const std::optional<H2DE_Face> possibleCollidedFace = redHitboxRect.getCollidedFace(topGroundRect);

        if (canSnap(possibleCollidedFace)) {
            snapPlayerHitboxToItemHitbox(playerRedHitboxOffset, redHitboxRect, topGroundRect, possibleCollidedFace.value());
            velocity.y = 0.0f;
            onSolid = true;
        }
    }
}

void Player::updateItemsCollisions() {
    H2DE_LevelRect playerRedHitboxRect = getCurrentRedHitboxWorldRect();
    H2DE_LevelRect playerBlueHitboxRect = getCurrentBlueHitboxWorldRect();

    for (const Item* item : level->getItems()) {
        if (dead) {
            break;
        }

        const Block* block = dynamic_cast<const Block*>(item);
        if (block != nullptr) {
            updateBlockCollisions(block, playerRedHitboxRect, playerBlueHitboxRect);
            continue;
        }

        const Trigger* trigger = dynamic_cast<const Trigger*>(item);
        if (trigger != nullptr) {
            updateTriggerCollisions(trigger, playerRedHitboxRect);
        }
    }
}

void Player::updateBlockCollisions(const Block* block, H2DE_LevelRect& playerRedHitboxRect, H2DE_LevelRect& playerBlueHitboxRect) {
    const BlockType& blockType = block->getType();
    if (blockType == BLOCK_TYPE_DECORATION) {
        return;
    }

    const std::optional<H2DE_LevelRect> possibleItemHitboxRect = block->getHitbox();
    if (!possibleItemHitboxRect.has_value()) {
        return;
    }

    const H2DE_LevelRect& itemHitboxRect = possibleItemHitboxRect.value();

    if (playerRedHitboxRect.collides(itemHitboxRect)) {
        const H2DE_LevelPos playerRedHitboxOffset = getCurrentRedHitboxOffset();
        bool snaped = updateBlockCollisions_redHitboxCollided(playerRedHitboxRect, playerRedHitboxOffset, itemHitboxRect, blockType);

        if (snaped) {
            return;
        }
    }

    if (playerBlueHitboxRect.collides(itemHitboxRect)) {
        const H2DE_LevelPos playerBlueHitboxOffset = getCurrentBlueHitboxOffset();
        bool snaped = updateBlockCollisions_blueHitboxCollided(playerBlueHitboxRect, playerBlueHitboxOffset, itemHitboxRect, blockType);
    }
}

bool Player::updateBlockCollisions_redHitboxCollided(H2DE_LevelRect& playerRedHitboxRect, const H2DE_LevelPos& playerRedHitboxOffset, const H2DE_LevelRect& itemHitboxRect, BlockType blockType) {
    return updateBlockCollisions_hitboxCollided(playerRedHitboxRect, playerRedHitboxOffset, "red", itemHitboxRect, blockType);
}

bool Player::updateBlockCollisions_blueHitboxCollided(H2DE_LevelRect& playerBlueHitboxRect, const H2DE_LevelPos& playerBlueHitboxOffset, const H2DE_LevelRect& itemHitboxRect, BlockType blockType) {
    return updateBlockCollisions_hitboxCollided(playerBlueHitboxRect, playerBlueHitboxOffset, "blue", itemHitboxRect, blockType);
}

bool Player::updateBlockCollisions_hitboxCollided(H2DE_LevelRect& playerHitboxRect, const H2DE_LevelPos& playerHitboxOffset, const std::string& playerHitboxColor, const H2DE_LevelRect& itemHitboxRect, BlockType blockType) {
    static const Data* gameData = game->getData();
    
    const std::optional<H2DE_Face> possibleCollidedFace = playerHitboxRect.getCollidedFace(itemHitboxRect);
    if (!possibleCollidedFace.has_value()) {
        return false;
    }

    const H2DE_Face collidedFace = possibleCollidedFace.value();

    switch (blockType) {
        case BLOCK_TYPE_SOLID: {
            bool isPlayerHitboxRed = (playerHitboxColor == "red");

            if (isPlayerHitboxRed) {

                if (canSnap(collidedFace)) {
                    snapPlayerHitboxToItemHitbox(playerHitboxOffset, playerHitboxRect, itemHitboxRect, collidedFace);
                    onSolid = true;
                    return true;
                }

            } else {
                snapPlayerHitboxToItemHitbox(playerHitboxOffset, playerHitboxRect, itemHitboxRect, collidedFace);
                kill();
                return true;
            }

            break;
        }

        case BLOCK_TYPE_OBSTACLE:
            snapPlayerHitboxToItemHitbox(playerHitboxOffset, playerHitboxRect, itemHitboxRect, collidedFace);
            kill();
            return true;
        
        default: break;
    }

    return false;
}

void Player::updateTriggerCollisions(const Trigger* trigger, H2DE_LevelRect& playerRedHitboxRect) {
    static const float& triggerSize = game->getData()->getTriggerSize();
    static H2DE_LevelRect triggerHitbox = { 0.0f, 0.0f, triggerSize, triggerSize };

    bool isTouchTrigger = trigger->getTouchTrigger();
    if (isTouchTrigger) {
        triggerHitbox.addPos(trigger->getPos());

        if (triggerHitbox.collides(playerRedHitboxRect)) {
            trigger->activate(level);
        }
        
    } else {
        if (pos.x >= trigger->getPos().x) {
            trigger->activate(level);
        }
    }
}

// rotation
void Player::updateRotation() {
    /** 
     * - 3: CALUL DE LA ROTATION :
     *          but: une fois que la position et la velocity son surs, on peut determiner la rotation de l'icon
     */

    if (!dead) {
        updateRotationOnSolid();
        updateRotationInAir();
        updateClampRotation();
        updateObjectRotation();
    }
}

void Player::updateRotationOnSolid() {
    static const Data* gameData = game->getData();

    if (!onSolid) {
        return;
    }
    
    const float remain = (gravity == PLAYER_GRAVITY_NORMAL)
        ? 90.0f - fmod(std::abs(rotation), 90.0f)
        : fmod(std::abs(rotation), 90.0f);

    const float& rotationIncr = gameData->getRotation(PLAYER_GAMEMODE_CUBE, size);
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
            rotation = pow(abs(velocity.y / maxGravity), 1.15f) * (velocity.y / maxGravity < 0 ? -1 : 1) * gamemodeRotation;
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
    H2DE_SetObjectRotation(getCurrentGamemodeObject(), rotation);
}

// ACTIONS
void Player::kill() {
    dead = true;

    for (const auto& [name, surface] : H2DE_GetBasicObjectSurfaces(getCurrentGamemodeObject())) {
        H2DE_HideSurface(surface);
    }

    level->playerDied();
}

void Player::click() {
    static const Data* gameData = game->getData();
    const float& click = gameData->getClick(gamemode, size);

    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE: {
            if (isOnSolid()) {
                velocity.y = -click * gravity;
            }
            break;
        }

        case PLAYER_GAMEMODE_SHIP: break;

        default: break;
    }
}

void Player::respawn(const Checkpoint& checkpoint) {
    static const Data* gameData = game->getData();

    showGamemodeSurfaces();

    onSolid = false;
    dead = false;

    const H2DE_LevelSize redHitboxSize = gameData->getPlayerHitbox(checkpoint.gamemode, checkpoint.size).red.getSize();

    pos = game->convertToLevelPos(checkpoint.pos, redHitboxSize);
    velocity.y = checkpoint.velocityY;
    rotation = checkpoint.rotation;

    gamemode = checkpoint.gamemode;
    size = checkpoint.size;
    gravity = checkpoint.gravity;
}

void Player::showGamemodeSurfaces() const {
    if (!dead) {
        return;
    }

    const std::vector<std::unordered_map<std::string, H2DE_Surface*>> gamemodesSurfaces = {
        H2DE_GetBasicObjectSurfaces(cube),
        // H2DE_GetBasicObjectSurfaces(ship),
    };

    for (const auto& surfaces : gamemodesSurfaces) {
        for (const auto& [name, surface] : surfaces) {
            H2DE_ShowSurface(surface);
        }
    }
}

void Player::snapPlayerHitboxToItemHitbox(const H2DE_LevelPos& hitboxOffset, H2DE_LevelRect& playerHitbox, const H2DE_LevelRect& itemHitbox, H2DE_Face face) {
    switch (face) {
        case H2DE_FACE_TOP:
            pos.y = itemHitbox.y + itemHitbox.h - hitboxOffset.y;
            break;

        case H2DE_FACE_BOTTOM:
            pos.y = itemHitbox.y - playerHitbox.h - hitboxOffset.y;
            break;

        case H2DE_FACE_RIGHT:
            pos.x = itemHitbox.x - playerHitbox.w - hitboxOffset.x;
            break;

        case H2DE_FACE_LEFT: {
            float overlapTop = itemHitbox.y + itemHitbox.h - playerHitbox.y;
            float overlapBottom = playerHitbox.y + playerHitbox.h - itemHitbox.y;

            float minOverlap = std::min(overlapTop, overlapBottom);

            if (minOverlap == overlapTop) {
                return snapPlayerHitboxToItemHitbox(hitboxOffset, playerHitbox, itemHitbox, H2DE_FACE_TOP);

            } else {
                return snapPlayerHitboxToItemHitbox(hitboxOffset, playerHitbox, itemHitbox, H2DE_FACE_BOTTOM);
            }
            break;
        }

        default: return;
    }

    velocity.y = 0.0f;
}

// GETTER
H2DE_BasicObject* Player::getCurrentGamemodeObject() const {
    switch (gamemode) {
        case PLAYER_GAMEMODE_CUBE: return cube;
        case PLAYER_GAMEMODE_SHIP: return ship;
        default: return nullptr;
    }
}

H2DE_LevelRect Player::getCurrentRedHitboxWorldRect() const {
    return getCurrentHitboxWorldRect("red");
}

H2DE_LevelRect Player::getCurrentBlueHitboxWorldRect() const {
    return getCurrentHitboxWorldRect("blue");
}

H2DE_LevelRect Player::getCurrentHitboxWorldRect(const std::string& color) const {
    return H2DE_GetObjectHitboxWorldRect(getCurrentGamemodeObject(), color);
}

H2DE_LevelPos Player::getCurrentRedHitboxOffset() const {
    return getCurrentHitboxOffset("red");
}

H2DE_LevelPos Player::getCurrentBlueHitboxOffset() const {
    return getCurrentHitboxOffset("blue");
}

H2DE_LevelPos Player::getCurrentHitboxOffset(const std::string& color) const {
    return H2DE_GetObjectHitbox(getCurrentGamemodeObject(), color).rect.getPos();
}

bool Player::isOnSolid() const {
    if (isOnGround()) {
        return true;
    }

    return isOnBlock();
}

bool Player::isOnGround() const {
    const H2DE_LevelRect botGroundRect = H2DE_GetObjectRect(level->getBotGround());
    const H2DE_LevelRect topGroundRect = H2DE_GetObjectRect(level->getTopGround());

    H2DE_LevelRect playerRedHitboxRect = getCurrentRedHitboxWorldRect();
    playerRedHitboxRect.y += (gravity == PLAYER_GRAVITY_NORMAL) ? 0.001f : -0.001f;

    if (playerRedHitboxRect.collides(botGroundRect)) {
        if (canSnap(playerRedHitboxRect.getCollidedFace(botGroundRect))) {
            return true;
        }
    }

    if (playerRedHitboxRect.collides(topGroundRect)) {
        if (canSnap(playerRedHitboxRect.getCollidedFace(topGroundRect))) {
            return true;
        }
    }

    return false;
}

bool Player::isOnBlock() const {
    H2DE_LevelRect playerRedHitboxRect = getCurrentRedHitboxWorldRect();
    playerRedHitboxRect.y += (gravity == PLAYER_GRAVITY_NORMAL) ? 0.001f : -0.001f;

    for (const Item* item : level->getItems()) {
        const Block* block = dynamic_cast<const Block*>(item);
        if (block == nullptr) {
            continue;
        }

        const BlockType& blockType = block->getType();
        if (blockType == BLOCK_TYPE_DECORATION) {
            continue;
        }

        const std::optional<H2DE_LevelRect> possibleItemHitboxRect = block->getHitbox();
        if (!possibleItemHitboxRect.has_value()) {
            continue;
        }

        const H2DE_LevelRect& itemHitboxRect = possibleItemHitboxRect.value();

        if (playerRedHitboxRect.collides(itemHitboxRect)) {
            if (canSnap(playerRedHitboxRect.getCollidedFace(itemHitboxRect))) {
                return true;
            }
        }
    }

    return false;
}

bool Player::canSnap(const std::optional<H2DE_Face>& possibleCollidedFace) const {
    static const Data* gameData = game->getData();

    if (!possibleCollidedFace.has_value()) {
        return false;
    }

    const H2DE_Face& collidedFace = possibleCollidedFace.value();
    const PlayerSnap& playerSnap = gameData->getPlayerSnap(gamemode, size);

    const H2DE_Face bottomFace = (gravity == PLAYER_GRAVITY_NORMAL) ? H2DE_FACE_BOTTOM : H2DE_FACE_TOP;
    const H2DE_Face topFace = (bottomFace == H2DE_FACE_BOTTOM) ? H2DE_FACE_TOP : H2DE_FACE_BOTTOM;

    bool isFalling = (gravity == PLAYER_GRAVITY_NORMAL) ? (velocity.y > 0.0f) : (velocity.y < 0.0f);

    bool canSnapOnBottomFace = (playerSnap.bottom && collidedFace == bottomFace && isFalling);
    bool canSnapOnTopFace = (playerSnap.top && collidedFace == topFace && !isFalling);

    return (canSnapOnBottomFace || canSnapOnTopFace);
}

const H2DE_LevelPos Player::getObjectPos() const {
    return H2DE_GetObjectPos(getCurrentGamemodeObject());
}

// SETTER 
void Player::setVelocityX(float vx) {
    velocity.x = vx;
}

void Player::setMouseDown(bool state) {
    mouseDown = state;
}
