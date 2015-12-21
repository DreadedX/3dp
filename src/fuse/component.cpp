#include "fuse/fuse.h"

void fuse::Component::super(Entity *entity) {

    assert(parent == nullptr);
    parent = entity;
};

uint fuse::_getUniqueID() {

    static uint id = 0;

    assert(id < MAX_COMPONENTS /*Maximum component count exceeded*/);

    return id++;
}
