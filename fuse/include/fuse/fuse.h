#ifndef FUSE_H
#define FUSE_H

#include <vector>
#include <algorithm>
#include <utility>
#include <array>
#include <bitset>
#include <cassert>

// #include "extra/extra.h"

#ifndef MAX_COMPONENTS
    #define MAX_COMPONENTS 32
#endif

#include "fuse/component.h"
#include "fuse/entity.h"

namespace fuse {

    void update();
    void draw();

    Entity *createEntity(const char *name = "Entity", Entity *parent = nullptr);
    void killAll();

    std::vector<Entity *> *getEntities();
}

#endif

