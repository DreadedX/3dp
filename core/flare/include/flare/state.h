#ifndef FLARE_STATE_H
#define FLARE_STATE_H

#include <string>

#include <GLFW/glfw3.h>

#include "extra/allocator/proxy_allocator.h"
#include "extra/allocator/free_list_allocator.h"
#include "flare/game_state.h"
#include "flare/assets/model.h"

namespace flare {

	struct Timer {

		Timer(std::string name, float time) {

			this->name = name;
			this->time = time;
		}

		std::string name;
		float time;
	};

	struct State {

		struct ProxyAllocators {

			ProxyAllocator *flux = nullptr;
			ProxyAllocator *fuse = nullptr;
			ProxyAllocator *asset = nullptr;
			ProxyAllocator *model = nullptr;
		} proxyAllocators;

		std::vector<Timer> timers;

		FreeListAllocator *mainAllocator = nullptr;

		GameState *mainState;

		struct Settings {

			/** @brief Window name */
			const char *name = "flare engine";
			/** @brief Window resolution */
			glm::ivec2 resolution = glm::ivec2(1280, 720);
			/** @brief Sensitity of the mouselook
			  @todo Camera controls should not be part of the engine */
			float mouseSensitivity = 1.0f;

			/** @brief All possible swap values */
			enum {
				/** @brief No v-sync */
				VSYNC_OFF = 0,
				/** @brief Normal v-sync */
				VSYNC_ON,
				/** @brief Half refresh rate v-sync */
				VSYNC_HALF
			};
			/** @brief V-Sync settings */
			int swap = VSYNC_OFF;
		} settings;

		GLFWwindow *window = nullptr;

		struct Render {

			/** @brief View matrix */
			glm::mat4 view;
			/** @brief Projection matrix */
			glm::mat4 projection;

			// MSAA Sample count
			int sampleCount;

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
			} light;

			// ~Render() {
			// 	for(flare::render::passes::Pass *pass : renderPasses) {
            //
			// 		delete pass;
			// 		pass = nullptr;
			// 	}
			// 	renderPasses.clear();
			// }

			/** @brief The currently used vbo */
			GLuint vbo = 0;
			/** @brief The currently used vao */
			GLuint vao = 0;
			/** @brief The currently used shader */
			flare::asset::Shader *shader = nullptr;
			std::unordered_map<std::string, GLuint> shaderOutput;

			/** @todo This needs to go */
			asset::Model *quad = nullptr;

			// Array<flare::render::passes::Pass*> renderPasses;

			/** @brief Frame delta time */
			float deltaTime = 0;

			/** @brief Camera data
			  @todo Move pitch and yaw from mouse to here
			  @todo This needs an initializer */
			struct Camera {

				/** @brief Camera position */
				glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
				/** @brief Camera roation (yaw, pitch, roll)
				  @note Currently roll does not do anything */
				glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
				/** @brief Vector pointing straight ahead from the camera */
				glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
				/** @brief Vector pointing straight up from the camera */
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
			} camera;
		} render;
	};
};

#endif

