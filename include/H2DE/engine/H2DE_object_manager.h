#ifndef H2DE_OBJECT_MANAGER_H
#define H2DE_OBJECT_MANAGER_H

#include <H2DE/engine/H2DE_engine.h>
class H2DE_Engine;
class H2DE_ButtonObject;

class H2DE_ObjectManager {
private:
    H2DE_Engine* engine;
    std::vector<H2DE_ButtonObject*> buttons = {};

    H2DE_ButtonObject* mouseDown = nullptr;
    H2DE_ButtonObject* hovered = nullptr;

    H2DE_ObjectManager(H2DE_Engine* engine);
    ~H2DE_ObjectManager() noexcept = default;

    void handleEvents(SDL_Event event);
    void handleMouseDownEvents(SDL_Event event);
    void handleMouseUpEvents(SDL_Event event);
    void handleHoverEvents(SDL_Event event);
    void handleBlurEvents(SDL_Event event);

    void refreshButtonBuffer(const std::vector<H2DE_Object*>& objects);

    const std::vector<H2DE_ButtonObject*> getValidButtons() const;

    friend class H2DE_Engine;
};

#endif
