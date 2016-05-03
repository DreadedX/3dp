#ifndef PLUGIN_OBJ_H
#define PLUGIN_OBJ_H

#include "flare/flare.h"

namespace obj {

		struct Model;

		/** @brief Structs needed for models */
		namespace model {

			/** @brief Mesh data
				@todo Make this use a material instead of storing the texture on its own */
			struct Mesh {

				glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
				glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
				GLfloat shininess = 0.0f;

				std::string diffuseMap = "";
				std::string specularMap = "";

				/** @brief List of all the vertices in this mesh */
				std::vector<flare::asset::model::Vertex> vertices;
				/** @brief List of all the indices in this mesh */
				std::vector<GLuint> indices;
			};
		}

		/** @brief Asset struct containing 3d model info */
		struct Model : flare::asset::Asset {

			/** @brief List of all meshes in this model */
			std::vector<model::Mesh*> meshes;

			/** @brief Load model data */
			void _load() override {}

			/** @brief Model destructor
				Makes sure all meshes get deleted properly */
			~Model() {

				/** @todo Should probably also delete opengl buffers (this currently only gets called on shutdown, so causes no issues */
				for (model::Mesh *mesh : meshes) {

					delete mesh;
				}
			}
		};

	Model read(const char *name);
}

#endif

