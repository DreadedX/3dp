#ifndef MODEL_H
#define MODEL_H

// TODO: This is really bad
namespace flare {
    namespace component {
	struct Object;
    }
}

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

		void _draw(Shader *shader, Material *material, component::Object *parent);
	    };
	}

	struct Model : Asset {

	    std::vector<model::Mesh*> meshes;

	    void draw(Shader *shader, Material *material, component::Object *parent);

	    void _load() override;
	};
    }
}

#endif

