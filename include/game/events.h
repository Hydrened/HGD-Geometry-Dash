#ifndef EVENTS_H
#define EVENTS_H

#include "game.h"
class Game;

class Events {
public:
    Events(Game* g) : game(g) {}
    ~Events() = default;

    void handle(SDL_Event event);

private:
    Game* game;

    void handle_keydown(SDL_Keycode keycode);
    void handle_keydown_menu(SDL_Keycode keycode);
    void handle_keydown_menu_modal(SDL_Keycode keycode);
    void handle_keydown_level(SDL_Keycode keycode);
    void handle_keyup(SDL_Keycode keycode);
    void handle_keyup_level(SDL_Keycode keycode);
    void handle_button_down(Uint8 button);
    void handle_button_down_level(Uint8 button);
    void handle_button_up(Uint8 button);
    void handle_button_up_level(Uint8 button);

    inline Menu* getMenu() const { return game->menu; }
    inline Level* getLevel() const { return game->level; }
    inline GameState getGameState() const { return game->state; }
    inline bool isInTransition() const { return game->inTransition; }
};

#endif
