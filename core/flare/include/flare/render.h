#ifndef FLARE_RENDER_H
#define FLARE_RENDER_H

namespace flare {
    namespace render {

	// TODO: This needs an initializer
	struct Camera {

	    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
	    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	};

	struct Light {
	    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

	    glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	    glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	    glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	struct State {

	    glm::mat4 view;
	    glm::mat4 projection;

	    Light light;

	    // The currently used
	    GLuint vbo = 0;
	    GLuint vao = 0;
	    GLuint shader = 0;

	    struct {
		GLuint diffuse = 0;
		GLuint specular = 0;
	    } material;

	    // Store frameTime
	    // TODO: There is probably a better location to store this
	    float deltaTime = 0;

	    Camera camera;
	};

	void init();
	void update();

	void setShader(asset::Shader *shader);
	void setModel(asset::Model *model);
	// TODO: This needs support for more than one texture
	void setMaterial(asset::Material *material);

	State *getState();
	Camera *getCamera();
	float getDeltaTime();

    }
}

#endif

