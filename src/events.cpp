#include "events.h"

#include "menu/menu.h"
#include "menu/modal.h"
#include "level/level.h"
#include "level/player.h"

// EVENTS
void Events::handle(SDL_Event event) {
    static H2DE_Engine* engine = game->getEngine();

    switch (event.type) {

        case SDL_KEYDOWN:
            handle_keydown(event.key.keysym.sym);
            switch (event.key.keysym.sym) {
                case SDLK_d: engine->toggleDebugMode(); break;
                case SDLK_LEFT: engine->debugModePreviousFrame(); break;
                case SDLK_RIGHT: engine->debugModeNextFrame(); break;
                default: break;
            } break;

        case SDL_KEYUP:
            handle_keyup(event.key.keysym.sym);
            break;

        case SDL_MOUSEBUTTONDOWN:
            handle_button_down(event.button.button);
            break;

        case SDL_MOUSEBUTTONUP:
            handle_button_up(event.button.button);
            break;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST && getGameState() == GAME_STATE_LEVEL) {
                engine->pause();
            }
            break;

        default: return;
    }
}

void Events::handle_keydown(SDL_Keycode keycode) {
    if (isInTransition()) {
        return;
    }

    switch (getGameState()) {
        case GAME_STATE_MENU: handle_keydown_menu(keycode); break;
        case GAME_STATE_LEVEL: handle_keydown_level(keycode); break;
        default: return;
    }
}

void Events::handle_keydown_menu(SDL_Keycode keycode) {
    bool isAMenuOpened = (getMenu() != nullptr);
    if (!isAMenuOpened) {
        return;
    }
    
    if (getMenu()->isAModalOpened()) {
        return handle_keydown_menu_modal(keycode);
    }
    
    const MenuID menuID = getMenu()->getId();
    LevelMenu* levelMenu = dynamic_cast<LevelMenu*>(getMenu());

    switch (keycode) {

        case SDLK_ESCAPE: switch (menuID) {
            case MENU_ID_MAIN_MENU: getMenu()->openModal(MODAL_ID_QUIT_GAME); break;
            case MENU_ID_LEVEL_MENU: game->openMenu(MENU_ID_MAIN_MENU); break;
            case MENU_ID_ICON_MENU: game->openMenu(MENU_ID_MAIN_MENU); break;
            default: break;
        } break;

        case SDLK_SPACE: switch (menuID) {
            case MENU_ID_MAIN_MENU: game->openMenu(MENU_ID_LEVEL_MENU); break;
            case MENU_ID_LEVEL_MENU: game->openLevel(levelMenu->getLevelIndex()); break;
            default: break;
        } break;

        case SDLK_LEFT: switch (menuID) {
            case MENU_ID_LEVEL_MENU: levelMenu->left(); break;
            default: break;
        } break;

        case SDLK_RIGHT: switch (menuID) {
            case MENU_ID_LEVEL_MENU: levelMenu->right(); break;
            default: break;
        } break;

        case SDLK_i: switch (menuID) {
            case MENU_ID_MAIN_MENU: game->openMenu(MENU_ID_ICON_MENU); break;
            default: break;
        }

        default: return;
    }
}

void Events::handle_keydown_menu_modal(SDL_Keycode keycode) {
    if (keycode == SDLK_ESCAPE) {
        return getMenu()->closeModal();
    }

    for (const Data::ModalButtonBuffer& buttonBuffer : getMenu()->getModal()->getBuffer().buttons) {
        bool differentShortcut = (buttonBuffer.shortcut != keycode);
        bool hasNoCallback = !buttonBuffer.callback;

        if (differentShortcut || hasNoCallback) {
            continue;
        }

        buttonBuffer.callback();
        return;
    }
}

void Events::handle_keydown_level(SDL_Keycode keycode) {
    static H2DE_Engine* engine = game->getEngine();

    bool isALevelOpened = (getLevel() != nullptr);
    if (!isALevelOpened) {
        return;
    }

    switch (keycode) {

        case SDLK_ESCAPE:
            if (engine->isPaused()) {
                game->openMenu(MENU_ID_LEVEL_MENU);
            } else {
                engine->pause();
            }
            break;

        case SDLK_SPACE:
            if (engine->isPaused()) {
                engine->resume();
            } else {
                getLevel()->getPlayer()->setMouseDown(true);
            }
            break;

        case SDLK_UP:
            getLevel()->getPlayer()->setMouseDown(true);
            break;

        default: return;
    }
}

void Events::handle_keyup(SDL_Keycode keycode) {
    if (isInTransition()) {
        return;
    }

    switch (getGameState()) {
        case GAME_STATE_LEVEL: handle_keyup_level(keycode); break;
        default: return;
    }
}

void Events::handle_keyup_level(SDL_Keycode keycode) {
    bool isALevelOpened = (getLevel() != nullptr);
    if (!isALevelOpened) {
        return;
    }

    switch (keycode) {

        case SDLK_SPACE:
            getLevel()->getPlayer()->setMouseDown(false);
            break;

        case SDLK_UP:
            getLevel()->getPlayer()->setMouseDown(false);
            break;

        default: return;
    }
}

void Events::handle_button_down(Uint8 button) {
    if (isInTransition()) {
        return;
    }

    switch (getGameState()) {
        case GAME_STATE_LEVEL: handle_button_down_level(button); break;
        default: return;
    }
}

void Events::handle_button_down_level(Uint8 button) {
    bool isALevelOpened = (getLevel() != nullptr);
    if (!isALevelOpened) {
        return;
    }

    switch (button) {

        case SDL_BUTTON_LEFT:
            getLevel()->getPlayer()->setMouseDown(true);
            break;

        default: return;
    }
}

void Events::handle_button_up(Uint8 button) {
    if (isInTransition()) {
        return;
    }

    switch (getGameState()) {
        case GAME_STATE_LEVEL: handle_button_up_level(button); break;
        default: return;
    }
}

void Events::handle_button_up_level(Uint8 button) {
    bool isALevelOpened = (getLevel() != nullptr);
    if (!isALevelOpened) {
        return;
    }

    switch (button) {

        case SDL_BUTTON_LEFT:
            getLevel()->getPlayer()->setMouseDown(false);
            break;

        default: return;
    }
}
