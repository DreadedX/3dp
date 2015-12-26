#ifndef FLARE_RENDER_H
#define FLARE_RENDER_H

namespace flare {
    namespace render {

	struct Camera {

	    glm::vec3 posiion = glm::vec3(0.0f, 0.0f, 3.0f);
	    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	};

	struct State {

	    GLuint vbo = 0;
	    GLuint vao = 0;
	    GLuint vaoLamp = 0;

	    float deltaTime = 0;

	    // TODO: This needs an initializer
	    Camera camera;
	};

	void init();
	void update();

	State *getState();
	Camera *getCamera();
	float getDeltaTime();

    }
}

#endif

