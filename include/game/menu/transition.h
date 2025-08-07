#pragma once

#include "utils.h"
class Game;

class Transition {
public:
    Transition(Game* game, TransitionState state, const std::function<void()>& completed);
    ~Transition() = default;
    
private:
    Game* game;
    TransitionState state;
    std::function<void()> completed = nullptr;
};
