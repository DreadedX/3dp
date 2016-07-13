#ifndef PROPERTIES_H
#define PROPERTIES_H

namespace flare {
	namespace component {

		/** Component that stores position data of the parent entity */
		struct Position : fuse::Component {

			glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

			Position(fuse::Entity *parent, glm::vec3 position) {
				super(parent);

				this->position = position;
			}
		};

		struct Scale : fuse::Component {

			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

			Scale(fuse::Entity *parent, glm::vec3 scale) {
				super(parent);

				this->scale = scale;
			}
		};

		/** Component that stores roation data of the parent entity */
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

