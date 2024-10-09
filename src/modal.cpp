#include "modal.h"

// INIT
Modal::Modal(Game* g, GameState s) : game(g), state(s) {
    game->setState(s);

    H2DE_Timeline* open = H2DE_CreateTimeline(game->getEngine(), game->getTransitionDuration(150), EASE_OUT, [this](float blend) {
        this->scaleMultiplier = blend;
    }, NULL, 0);

    H2DE_AddTimelineToManager(tm, open);
}

// CLEANUP
Modal::~Modal() {
    H2DE_DestroyTimelineManager(tm);
}

// UPDATE
void Modal::update() {
    H2DE_TickTimelineManager(tm);
}

// RENDER
void Modal::render() {
    static H2DE_Engine* engine = game->getEngine();
    GameState currentState = game->getState();

    if (state == currentState) {
        H2DE_Size engineSize = H2DE_GetEngineSize(engine);

        H2DE_GraphicObject* modal = H2DE_CreateGraphicObject();
        modal->type = IMAGE;
        modal->texture = "1_1.png";
        modal->pos = { engineSize.w / 2 - 250, engineSize.h / 2 - 250 };
        modal->size = { 500, 500 };
        modal->scaleOrigin = { 250, 250 };
        modal->scale = { scaleMultiplier, scaleMultiplier };
        modal->index = 10000;
        H2DE_AddGraphicObject(engine, modal);
    } 
}
