#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "game.h"

class Game;

class Calculator {
private:
    Game* game;

    void getUtils(H2DE_Size* enginSize, int* blockSize);

public:
    Calculator(Game* game);
    ~Calculator();

    H2DE_Pos convertToPx(LevelPos pos, LevelSize size, bool lockX, bool lockY);
    H2DE_Size convertToPx(LevelSize size);
    H2DE_Pos convertToPx(LevelOffset offset);
    H2DE_Pos convertToPxAbs(LevelPos pos);
    H2DE_Size convertToPxAbs(LevelSize size);
};

#endif
