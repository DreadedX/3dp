#ifndef MATERIAL_H
#define MATERIAL_H

namespace flare {

    namespace asset {

	/** @brief Asset struct containing material data */
	struct Material : Asset {

		// diffuse color				Kd
		// diffuse specular color		Kd
		// shininess					Ns
		// diffuse map					map_Kd
		// specular map					map_Ks
		
		/** @brief The diffuse color of the material */
		glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		/** @brief The specular color of the material */
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		/** @brief The shininess of the material */
	    GLfloat shininess = 0.0f;
		/** @brief Pointer to diffuse Texture Asset */
	    Texture *diffuse = nullptr;
		/** @brief Pointer to specular Texture Asset */
	    Texture *specular = nullptr;

		/** @brief Load material data */
	    void _load() override;
	};
    }
}

#endif

