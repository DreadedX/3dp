#ifndef MODEL_H
#define MODEL_H

#include "glm/glm.hpp"

#include "flare/assets/texture.h"

namespace flare {

	namespace asset {

		struct Model;

		/** @brief Structs needed for models */
		namespace model {

			/** @brief Vertex data */
			struct Vertex {
				/** @brief Vertex position */
				glm::vec3 position;
				/** @brief Vertex normal */
				glm::vec3 normal;
				/** @brief Vertex tangent
					@note These are not needed for basic rendering */
				// glm::vec3 tangent;
				/** @brief Vertex texture coords */
				glm::vec2 texCoords;
			};

			/** @brief Mesh data
				@todo Make this use a material instead of storing the texture on its own */
			struct Mesh {

				/** @brief Id of vertex array object */
				GLuint vao = 0;
				/** @brief Id of vertex buffer object */
				GLuint vbo = 0;
				/** @brief Id of idex buffer object */
				GLuint ebo = 0;
				
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

				/** @brief List of all the vertices in this mesh */
				model::Vertex *vertices = nullptr;
				/** @brief List of all the indices in this mesh */
				GLuint *indices = nullptr;
				GLuint indexSize = 0;
			};
		}

		/** @brief Asset struct containing 3d model info */
		struct Model : Asset {

			/** @brief List of all meshes in this model */
			Array<model::Mesh*> meshes;

			/** @brief Load model data */
			void _load() override;

			/** @brief Model destructor
				Makes sure all meshes get deleted properly */
			~Model();
		};
	}
}

#endif

