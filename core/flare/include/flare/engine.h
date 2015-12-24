#ifndef FLARE_ENGINE_H
#define FLARE_ENGINE_H

namespace flare {

    void init();
    bool isRunning();
    void update();
    void terminate(int errorCode = 0);
}

#endif

