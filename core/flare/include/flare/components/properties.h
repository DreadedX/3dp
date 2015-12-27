#ifndef PROPERTIES_H
#define PROPERTIES_H

namespace flare {
    namespace component {

	struct Position : fuse::Component {

	    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

	    Position(fuse::Entity *parent, glm::vec3 position) {
		super(parent);

		this->position = position;
	    }
	};

	struct Rotation : fuse::Component {

	    GLfloat rotation = 0;
	    glm::vec3 rotationAxis = glm::vec3(0.0f, 0.0f, 0.0f);

	    Rotation(fuse::Entity *parent, GLfloat rotation, glm::vec3 rotationAxis) {
		super(parent);

		this->rotation = rotation;
		this->rotationAxis = rotationAxis;
	    }
	};
    }
}

#endif

