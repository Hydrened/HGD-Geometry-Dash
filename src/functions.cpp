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

const int getIndex(Layer layer, int order) {
    constexpr int range = 100;
    return layer * range * 2 + layer + std::clamp(order, -range, range) + range;
}

const int getIndex(const std::pair<Layer, int>& index) {
    return getIndex(index.first, index.second);
}

const std::string formatID(int n, int length) {
    std::ostringstream oss;
    oss << std::setw(length) << std::setfill('0') << n;
    return oss.str();
}
