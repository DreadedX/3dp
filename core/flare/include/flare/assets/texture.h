#ifndef TEXTURE_H
#define TEXTURE_H

namespace flare {

    namespace asset {

	struct Texture : Asset {

	    GLuint id = 0;

	    void _load() override;
	};
    }
}

#endif

