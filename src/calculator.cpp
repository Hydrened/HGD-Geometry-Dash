#include "calculator.h"

// INIT
Calculator::Calculator(Game* g) : game(g) {
    
}

// CLEANUP
Calculator::~Calculator() {
    
}

// CONVERT
H2DE_Pos Calculator::convertToPx(LevelPos p, LevelSize s, bool lx, bool ly) {
    H2DE_Size engineSize;
    int blockSize;
    getUtils(&engineSize, &blockSize);
    LevelPos c = game->getCamera()->getPos();

    return {
        (int)((lx) ? p.x * blockSize : (p.x - c.x) * blockSize),
        (int)((ly) ? engineSize.h - (p.y + s.h) * blockSize : engineSize.h - (p.y - c.y + s.h) * blockSize)
    };
}

H2DE_Size Calculator::convertToPx(LevelSize s) {
    H2DE_Size engineSize;
    int blockSize;
    getUtils(&engineSize, &blockSize);

    return {
        (int)(s.w * blockSize),
        (int)(s.h * blockSize)
    };
}

H2DE_Pos Calculator::convertToPx(LevelOffset o) {
    H2DE_Size engineSize;
    int blockSize;
    getUtils(&engineSize, &blockSize);

    return {
        (int)(o.x * blockSize),
        (int)(o.y * blockSize)
    };
}

H2DE_Pos Calculator::convertToPxAbs(LevelPos p) {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_Size engineSize = H2DE_GetEngineSize(engine);

    return { (int)(p.x / 100 * engineSize.w), (int)(p.y / 100 * engineSize.h) };
}

H2DE_Size Calculator::convertToPxAbs(LevelSize s) {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_Size engineSize = H2DE_GetEngineSize(engine);

    return { (int)(s.w / 100 * engineSize.w), (int)(s.h / 100 * engineSize.h) };
}

// GETTER
void Calculator::getUtils(H2DE_Size* engineSize, int* blockSize) {
    *engineSize = H2DE_GetEngineSize(game->getEngine());
    *blockSize = engineSize->w / BLOCKS_ON_WIDTH;
}
