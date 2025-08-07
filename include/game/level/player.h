#pragma once

#include "utils.h"
#include "data.h"
class Game;
class Scenery;
class Block;
class Trigger;

class Player {
public:
    Player(Game* game, Level* level, Scenery* scenery, const Checkpoint& checkpoint, const PlayerIcons& icons);
    ~Player();

    void update();

    void click();
    void kill();
    void respawn();

    constexpr const H2DE_Translate getTranslate() const {
        return normalCube->getTranslate();
    }
    const Speed getSpeed() const;
    inline H2DE_LevelRect getCurrentRedHitboxWorldRect() const {
        return getCurrentHitboxWorldRect("red");
    }
    inline H2DE_LevelRect getCurrentBlueHitboxWorldRect() const {
        return getCurrentHitboxWorldRect("blue");
    }
    constexpr bool isDead() const {
        return dead;
    }

    inline void setVelocityX(float vx) { velocity.x = vx; }
    inline void setMouseDown(bool state) { mouseDown = state; }

private:
    Game* game;
    Level* level;
    Scenery* scenery;
    Checkpoint checkpoint;
    PlayerIcons icons;

    H2DE_BasicObject* normalCube = nullptr;
    H2DE_BasicObject* miniCube = nullptr;
    H2DE_BasicObject* normalShip = nullptr;
    H2DE_BasicObject* miniShip = nullptr;

    H2DE_Velocity velocity = { 0.0f, 0.0f };
    float rotation = 0.0f;

    PlayerGamemode gamemode = PLAYER_GAMEMODE_CUBE;
    PlayerSize size = PLAYER_SIZE_NORMAL;
    PlayerGravity gravity = PLAYER_GRAVITY_NORMAL;

    bool onSolid = false;
    bool mouseDown = false;
    bool dead = false;

    void initState();

    void createIcons();
    H2DE_BasicObject* createIcon(PlayerGamemode gamemode, PlayerSize size, IconID id);
    static void applySurfacesOnIcon(H2DE_BasicObject* icon, const std::vector<Data::IconSurfaceBuffer>& iconSurfacesBuffer);
    void applyHitboxesOnIcon(H2DE_BasicObject* icon, const PlayerHitbox& hitboxes) const;

    void destroyObjects();

    void updateReset();
    void updateGravity();
    void updateSpecialBlocksCollision();
    void updateClick();
    void updateTranslationFromVelocity();
    void updateGroundCollisions();
    void updateItemCollisions();
    void updateBlockCollisions();
    void updateBlockCollisions_hitboxCollided(const H2DE_LevelRect& playerHitboxRect, const H2DE_LevelRect& blockHitboxRect, const BlockType& blockType);
    void updateBlockCollisions_hitboxCollided_solid(const H2DE_LevelRect& playerHitboxRect, const H2DE_LevelRect& blockHitboxRect, const H2DE_Face& collidedFace);
    void updateTriggerCollisions();

    void updateRotation();
    void updateRotationOnSolid();
    void updateRotationInAir();
    void updateClampRotation();
    void updateObjectRotation();

    void snapHitboxRectToOtherHitboxRect(const H2DE_LevelRect& hitboxRect, const H2DE_LevelRect& otherHitboxRect, H2DE_Face face);

    bool canSnap(const std::optional<H2DE_Face>& possibleCollidedFace) const;
    bool isOnSolid() const;
    bool isOnGround() const;
    bool isOnBlock() const;

    H2DE_BasicObject* getCurrentGamemodeObject() const;
    H2DE_LevelRect getCurrentHitboxWorldRect(const std::string& color) const;
    inline const std::array<H2DE_BasicObject*, 4> getObjects() const {
        return { normalCube, miniCube, normalShip, miniShip };
    }

    void setTranslate(const H2DE_Translate& translate);
    void setGamemode(PlayerGamemode gamemode, int yEntry);
    void setSize(PlayerSize size);
    void setGravity(PlayerGravity gravity);
    void setRotation(float rotation);
};
