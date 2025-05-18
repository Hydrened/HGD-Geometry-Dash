#include "functions.h"

void callShortcut(SDL_Keycode keycode) {
    SDL_Event fakeEvent;
    fakeEvent.type = SDL_KEYDOWN;
    fakeEvent.key.type = SDL_KEYDOWN;
    fakeEvent.key.state = SDL_PRESSED;
    fakeEvent.key.repeat = 0;
    fakeEvent.key.keysym.sym = keycode;

    SDL_PushEvent(&fakeEvent);
}

const std::string formatID(int n, int length) {
    std::ostringstream oss;
    oss << std::setw(length) << std::setfill('0') << n;
    return oss.str();
}
