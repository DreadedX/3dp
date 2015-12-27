#ifndef MODEL_H
#define MODEL_H

namespace flare {

    namespace asset {

	struct Model;

	namespace model {

	    struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
	    };

	    struct Mesh {

		GLuint vao = 0;
		GLuint vbo = 0;
		GLuint ebo = 0;

		std::vector<model::Vertex> vertices;
		std::vector<GLuint> indices;
	    };
	}

	struct Model : Asset {

	    std::vector<model::Mesh*> meshes;

	    void _load() override;

	    ~Model() {

		for (model::Mesh *mesh : meshes) {

		    delete mesh;
		}
	    }
	};
    }
}

#endif

