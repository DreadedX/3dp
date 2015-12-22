#include "fuse/fuse.h"

#pragma push_macro("super")
#undef super

void fuse::Component::super(Entity *entity, const char *name) {
    
    this->name = name;

    assert(parent == nullptr);
    parent = entity;
};

#pragma pop_macro("super")

uint fuse::_getUniqueID() {

    static uint id = 0;

    assert(id < MAX_COMPONENTS /*Maximum component count exceeded*/);

    return id++;
}
