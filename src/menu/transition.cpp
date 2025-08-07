#include "menu/transition.h"

#include "game.h"

// INIT
Transition::Transition(Game* g, TransitionState s, const std::function<void()>& c) : game(g), state(s), completed(c) {
    static H2DE_Engine* engine = game->getEngine();

    const int transitionDuration = game->getSave()->getTransitionDuration();
    if (transitionDuration == 0) {
        if (completed) {
            completed();
        }

        game->inTransition = false;
        delete this;
        return;
    }

    game->inTransition = true;

    H2DE_ObjectData od = H2DE_ObjectData();
    od.transform.scale = { 1.1f, 1.1f };
    od.index = H2DE_INDEX_MAX;
    od.absolute = true;

    H2DE_BasicObject* transitionObject = engine->createObject<H2DE_BasicObject>(od);

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.transform.scale = engine->getCamera()->getInterfaceScale();

    const uint8_t defaultTransitionOpacity = (state == TRANSITION_STATE_IN) ? 255 : 0;
    const uint8_t finalTransitionOpacity = (state == TRANSITION_STATE_IN) ? 0 : 255;

    H2DE_ColorData cd = H2DE_ColorData();
    cd.color = { 0, 0, 0, defaultTransitionOpacity };

    H2DE_Color* colorSurface = transitionObject->addSurface<H2DE_Color>("main", sd, cd);

    colorSurface->setColor({ 0, 0, 0, finalTransitionOpacity }, transitionDuration, H2DE_EASING_EASE_OUT, [this, transitionObject]() {
        if (completed) {
            completed();
        }

        engine->destroyObject(transitionObject);
        game->inTransition = false;

        delete this;
    }, false);
}
