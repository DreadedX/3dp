#ifndef FLARE_ENGINE_H
#define FLARE_ENGINE_H

namespace flare {

    struct Settings {

	// NOTE: These are the default engine settigs.
	const char *name = "flare engine";
	glm::ivec2 resolution = glm::ivec2(1280, 720);
	float mouseSensitivity = 0.5f;
	int swap = 0;
    };

    void init();
    bool isRunning();
    void update();

    Settings *getSettings();

    void terminate(int errorCode = 0);
}

#endif

