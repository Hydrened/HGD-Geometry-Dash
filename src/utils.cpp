#include "utils.h"

#include <H2DE/H2DE.h>

void callShortcut(SDL_Keycode keycode) {
    SDL_Event fakeEvent;
    fakeEvent.type = SDL_KEYDOWN;
    fakeEvent.key.type = SDL_KEYDOWN;
    fakeEvent.key.state = SDL_PRESSED;
    fakeEvent.key.repeat = 0;
    fakeEvent.key.keysym.sym = keycode;

    SDL_PushEvent(&fakeEvent);
}

void openExternLink(const std::string& link) {
    std::wstring wlink(link.begin(), link.end());
    ShellExecuteW(nullptr, L"open", wlink.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

void logLoadingTime(const std::function<void()>& loading, const std::string& name) {
    if (!loading) {
        return;
    }

    Uint32 startTime = SDL_GetTicks();

    loading();

    Uint32 elapsedTime = SDL_GetTicks() - startTime;
    std::cout << "Loaded " << name << " in " << elapsedTime << " ms" << std::endl;
}
