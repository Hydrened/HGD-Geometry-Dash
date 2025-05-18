#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"
class Game;
class Level;
class Block;
class Trigger;

class Player {
private:
    Game* game;
    Level* level;
    Checkpoint checkpoint;

    H2DE_BasicObject* cube = nullptr;
    H2DE_BasicObject* ship = nullptr;

    H2DE_LevelPos pos = { 0.0f, 0.0f };
    H2DE_LevelVelocity velocity = { 0.0f, 0.0f };
    float rotation = 0.0f;

    PlayerGamemode gamemode = PLAYER_GAMEMODE_CUBE;
    PlayerSize size = PLAYER_SIZE_NORMAL;
    PlayerGravity gravity = PLAYER_GRAVITY_NORMAL;

    bool onSolid = false;
    bool mouseDown = false;
    bool dead = false;

    void initCheckpoint();
    void initIcons();
    void initIconCreateLayers(const std::string& textureName, const std::vector<std::tuple<std::string, H2DE_AbsRect, H2DE_LevelRect, H2DE_ColorRGB>>& layers, std::unordered_map<std::string, H2DE_Surface*>& surfaces) const;
    void initCube();
    void initShip();

    void destroyIcons();

    void updateVelocity();
    void updateGravity();
    void updateSpecialBlocksCollision();
    void updateClick();

    void updatePosition();
    void updateGroundsCollisions();
    void updateItemsCollisions();
    void updateBlockCollisions(const Block* block, H2DE_LevelRect& playerRedHitboxRect, H2DE_LevelRect& playerBlueHitboxRect);
    bool updateBlockCollisions_redHitboxCollided(H2DE_LevelRect& playerRedHitboxRect, const H2DE_LevelPos& playerRedHitboxOffset, const H2DE_LevelRect& itemHitboxRect, BlockType blockType);
    bool updateBlockCollisions_blueHitboxCollided(H2DE_LevelRect& playerBlueHitboxRect, const H2DE_LevelPos& playerBlueHitboxOffset, const H2DE_LevelRect& itemHitboxRect, BlockType blockType);
    bool updateBlockCollisions_hitboxCollided(H2DE_LevelRect& playerHitboxRect, const H2DE_LevelPos& playerHitboxOffset, const std::string& playerHitboxColor, const H2DE_LevelRect& itemHitboxRect, BlockType blockType);
    void updateTriggerCollisions(const Trigger* trigger, H2DE_LevelRect& playerRedHitboxRect);

    void updateRotation();
    void updateRotationOnSolid();
    void updateRotationInAir();
    void updateClampRotation();
    void updateObjectRotation();

    void showGamemodeSurfaces() const;

    void snapPlayerHitboxToItemHitbox(const H2DE_LevelPos& hitboxOffset, H2DE_LevelRect& playerHitbox, const H2DE_LevelRect& itemHitbox, H2DE_Face face);

    H2DE_BasicObject* getCurrentGamemodeObject() const;

    H2DE_LevelRect getCurrentRedHitboxWorldRect() const;
    H2DE_LevelRect getCurrentBlueHitboxWorldRect() const;
    H2DE_LevelRect getCurrentHitboxWorldRect(const std::string& color) const;

    H2DE_LevelPos getCurrentRedHitboxOffset() const;
    H2DE_LevelPos getCurrentBlueHitboxOffset() const;
    H2DE_LevelPos getCurrentHitboxOffset(const std::string& color) const;

    bool isOnSolid() const;
    bool isOnGround() const;
    bool isOnBlock() const;
    bool canSnap(const std::optional<H2DE_Face>& possibleCollidedFace) const;

public:
    Player(Game* game, Level* level, const Checkpoint& checkpoint);
    ~Player();

    void update();

    void kill();
    void click();
    void respawn(const Checkpoint& checkpoint);

    inline const H2DE_LevelPos getPos() const { return pos; }
    inline const H2DE_LevelVelocity getVelocity() const { return velocity; }
    const H2DE_LevelPos getObjectPos() const;

    void setVelocityX(float vx);
    void setMouseDown(bool state);
};

#endif
