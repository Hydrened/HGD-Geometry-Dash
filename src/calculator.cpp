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
        static_cast<int>((lx) ? p.x * blockSize : (p.x - c.x) * blockSize),
        engineSize.h - static_cast<int>((p.y - c.y + s.h) * blockSize)
    };
}

H2DE_Size Calculator::convertToPx(LevelSize s) {
    H2DE_Size engineSize;
    int blockSize;
    getUtils(&engineSize, &blockSize);

    return {
        static_cast<int>(s.w * blockSize),
        static_cast<int>(s.h * blockSize)
    };
}

H2DE_Pos Calculator::convertToPx(LevelOffset o) {
    H2DE_Size engineSize;
    int blockSize;
    getUtils(&engineSize, &blockSize);

    return {
        static_cast<int>(o.x * blockSize),
        static_cast<int>(o.y * blockSize)
    };
}

// GETTER
void Calculator::getUtils(H2DE_Size* engineSize, int* blockSize) {
    *engineSize = H2DE_GetEngineSize(game->getEngine());
    *blockSize = engineSize->w / BLOCKS_ON_WIDTH;
}
