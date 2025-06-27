#ifndef MENU_TRANSITION_H
#define MENU_TRANSITION_H

#include "utils.h"
class Game;

class MenuTransition {
public:
    MenuTransition(Game* game, TransitionState state, const std::function<void()>& completed);
    ~MenuTransition() = default;
    
private:
    Game* game;
    TransitionState state;
    std::function<void()> completed = nullptr;
};

#endif
