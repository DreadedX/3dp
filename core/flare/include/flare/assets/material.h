#ifndef MATERIAL_H
#define MATERIAL_H

namespace flare {

    namespace asset {

	/** @brief Asset struct containing material data
		@todo This is currently not used */
	struct Material : Asset {

		/** @brief Pointer to diffuse Texture Asset */
	    Texture *diffuse = nullptr;
		/** @brief Pointer to specular Texture Asset */
	    Texture *specular = nullptr;
		/** @brief Pointer to normal Texture Asset */
	    Texture *normal = nullptr;
		/** @brief Pointer to emission Texture Asset */
	    Texture *emission = nullptr;
		/** @brief The shininess of the material */
	    GLfloat shininess = 32.0f;

		/** @brief Load material data */
	    void _load() override;
	};
    }
}

#endif

