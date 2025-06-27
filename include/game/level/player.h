#ifndef PLAYER_H
#define PLAYER_H

#include "utils.h"
#include "data.h"
class Game;
class Scenery;

class Player {
public:
    Player(Game* game, Scenery* scenery, const Checkpoint& checkpoint, const PlayerIcons& icons);
    ~Player();

    void update();

    void click();

    const Speed getSpeed() const;

    inline void setVelocityX(float vx) { velocity.x = vx; }
    inline void setMouseDown(bool state) { mouseDown = state; }

private:
    Game* game;
    Scenery* scenery;
    Checkpoint checkpoint;
    PlayerIcons icons;

    H2DE_BasicObject* normalCube = nullptr;
    H2DE_BasicObject* miniCube = nullptr;
    H2DE_BasicObject* normalShip = nullptr;
    H2DE_BasicObject* miniShip = nullptr;

    H2DE_Translate translate = { 0.0f, 0.0f };
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
    H2DE_BasicObject* createIcon(PlayerGamemode gamemode, PlayerSize size, Icon_ID id);
    static void applySurfacesOnIcon(H2DE_BasicObject* icon, const std::vector<Data::IconSurfaceBuffer>& iconSurfacesBuffer);
    void applyHitboxesOnIcon(H2DE_BasicObject* icon, const PlayerHitbox& hitboxes) const;

    void destroyObjects();

    void updateVelocity();
    void updateGravity();
    void updateSpecialBlocksCollision();
    void updateClick();

    void updateTranslation();
    void updateTranslationAddVelocity();
    void updateGroundsColllisions();
    void updateBlockCollisions();

    void updateRotation();
    void updateRotationOnSolid();
    void updateRotationInAir();
    void updateClampRotation();
    void updateObjectRotation();

    void snapHitboxRectToOtherHitboxRect(const H2DE_LevelRect& hitboxRect, const H2DE_LevelRect& otherHitboxRect, H2DE_Face face);

    inline H2DE_Translate getTranslate() const { return normalCube->getTranslate(); }

    bool canSnap(const std::optional<H2DE_Face>& possibleCollidedFace) const;
    bool isOnSolid() const;
    bool isOnGround() const;
    bool isOnBlock() const;

    H2DE_BasicObject* getCurrentGamemodeObject() const;
    inline H2DE_LevelRect getCurrentRedHitboxWorldRect() const { return getCurrentHitboxWorldRect("red"); }
    inline H2DE_LevelRect getCurrentBlueHitboxWorldRect() const { return getCurrentHitboxWorldRect("blue"); }
    H2DE_LevelRect getCurrentHitboxWorldRect(const std::string& color) const;
    inline const std::array<H2DE_BasicObject*, 4> getObjects() const { return { normalCube, miniCube, normalShip, miniShip }; }

    void setTranslate(const H2DE_Translate& translate);
    void setGamemode(PlayerGamemode gamemode, int yEntry);
    void setSize(PlayerSize size);
    void setGravity(PlayerGravity gravity);
};

#endif
