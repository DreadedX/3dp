#ifndef FLARE_LOADER_H
#define FLARE_LOADER_H

namespace flare {

    namespace shader {

	GLuint load(std::string name);
    }

    namespace texture {

	GLuint load(std::string name);
    }
}

#endif

