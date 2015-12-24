#ifndef MANAGER_H
#define MANAGER_H

namespace fuse {

    void update();
    void draw();

    Entity *createEntity(const char *name = "Entity", Entity *parent = nullptr);
    void killAll();

    std::vector<Entity *> *getEntities();
}

#endif

