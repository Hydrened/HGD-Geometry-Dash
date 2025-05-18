#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "game.h"

inline std::pair<Layer, int> getIndexFromJson(const json& index) { return { static_cast<Layer>(index["layer"]), static_cast<int>(index["order"]) }; }

inline const int getIndex(Layer layer, int order) {
    constexpr int RANGE = 100;
    return layer * RANGE * 2 + layer + std::clamp(order, -RANGE, RANGE) + RANGE;
}
inline const int getIndex(const std::pair<Layer, int>& index) { return getIndex(index.first, index.second); }

void callShortcut(SDL_Keycode keycode);
const std::string formatID(int n, int length);

#endif
