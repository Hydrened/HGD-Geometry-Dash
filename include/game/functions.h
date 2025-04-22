#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "game.h"

inline H2DE_LevelPos getPosFromJson(const json& pos) {
    return H2DE_LevelPos{
        static_cast<float>(pos["x"]),
        static_cast<float>(pos["y"]),
    };
}

inline H2DE_LevelRect getLevelRectFromJson(const json& rect) {
    return H2DE_LevelRect{
        static_cast<float>(rect["x"]),
        static_cast<float>(rect["y"]),
        static_cast<float>(rect["w"]),
        static_cast<float>(rect["h"]),
    };
}

inline H2DE_AbsRect getAbsRectFromJson(const json& rect) {
    return H2DE_AbsRect{
        static_cast<int>(rect["x"]),
        static_cast<int>(rect["y"]),
        static_cast<int>(rect["w"]),
        static_cast<int>(rect["h"]),
    };
}

inline H2DE_ColorRGB getColorFromJson(const json& color) {
    return H2DE_ColorRGB{
        static_cast<Uint8>(color["r"]),
        static_cast<Uint8>(color["g"]),
        static_cast<Uint8>(color["b"]),
    };
}

inline std::pair<Layer, int> getIndexFromJson(const json& index) {
    return {
        static_cast<Layer>(index["l"]),
        static_cast<int>(index["o"]),
    };
}

void callShortcut(SDL_Keycode keycode);
const int getIndex(Layer layer, int order);
const int getIndex(const std::pair<Layer, int>& index);
const std::string formatID(int n, int length);

#endif
