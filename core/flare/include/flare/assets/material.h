#ifndef MATERIAL_H
#define MATERIAL_H

namespace flare {

    namespace asset {

	struct Material : Asset {

	    Texture *diffuse = nullptr;
	    Texture *specular = nullptr;
	    Texture *normal = nullptr;
	    Texture *emission = nullptr;
	    GLfloat shininess = 32.0f;

	    void _load() override;
	};
    }
}

#endif

