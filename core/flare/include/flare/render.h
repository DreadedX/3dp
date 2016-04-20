#ifndef FLARE_RENDER_H
#define FLARE_RENDER_H

namespace flare {

	/** @brief Render code */
	namespace render {

		/** @brief Camera data
			@todo Move pitch and yaw from mouse to here
			@todo This needs an initializer */
		struct Camera {

			/** @brief Camera position */
			glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
			/** @brief Vector pointing straight ahead from the camera */
			glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
			/** @brief Vector pointing straight up from the camera */
			glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		};

		/** @brief Global light settings
			@todo This should configurable */
		struct Light {
			/** @brief The direction the light is shining */
			glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);

			/** @brief Color of the ambient light */
			glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
			/** @brief Color of the diffuse light */
			glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			/** @brief Color of the specular light */
			glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
		};

		/** @brief OpenGL State */
		struct State {

			/** @brief View matrix */
			glm::mat4 view;
			/** @brief Projection matrix */
			glm::mat4 projection;

			/** @brief Object containing light data */
			Light light;

			/** @brief The currently used vbo */
			GLuint vbo = 0;
			/** @brief The currently used vao */
			GLuint vao = 0;
			/** @brief The currently used shader */
			GLuint shader = 0;

			struct {
				GLuint diffuse = 0;
				GLuint specular = 0;
			} material;

			/** @brief Frame delta time */
			float deltaTime = 0;

			/** @brief Object containing camera data */
			Camera camera;
		};

		/** @brief Initialize OpenGL and renderer */
		void init();

		/** @brief Draw a new frame */
		void update();

		/** @brief Set the shader
			@param shader - Pointer to Shader Asset to use */
		void setShader(asset::Shader *shader);
		/** @brief Set the model to render
			@param model - Pointer to Model Asset to use */
		void setModel(asset::Model *model);
		/** @brief Set the material
			@todo This needs to be intergrated into object.cpp, because right now it is not used */
		void setMaterial(asset::Material *material);

		/** @brief Get the current OpenGL state
			@returns Pointer to the state */
		State *getState();

		/** @brief Get the camera data
			@returns Pointer to the camera */
		Camera *getCamera();

		/** @brief Get the frame delta time
			@returns The delta frame time */
		float getDeltaTime();

	}
}

#endif

