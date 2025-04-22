#ifndef SAVE_H
#define SAVE_H

#include "game.h"

class Save {
private:
    json data;

    void initFile();
    void loadData();

public:
    Save();
    ~Save();

    const int getTransitionDuration() const;
};

#endif
