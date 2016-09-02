#ifndef PASSES_H
#define PASSES_H

#include "glm/glm.hpp"

#include "flare/assets/shader.h"
#include "flare/assets/skybox.h"

namespace flare {

	class GameState;

	namespace render::passes {

		struct Pass {

			GLuint fbo;
			Array<GLuint> textures;

			asset::Shader *shader = nullptr;

			virtual void init() {}
			virtual void draw(GameState *) {}

			virtual ~Pass() {}
		};

		struct Basic : Pass {

			void init() override;
			void draw(GameState *) override;
		};

		struct Shadow : Pass {

			void init() override;
			void draw(GameState *) override;
		};

		struct Skybox : Pass {

			asset::Skybox *skybox = nullptr;
			GLuint vao;

			void init() override;
			void draw(GameState *) override;
		};

		struct ResolveMSAA : Pass {

			void init() override;
			void draw(GameState *) override;
		};

		struct Geometry : Pass {

			void init() override;
			void draw(GameState *) override;
		};

		struct SSAO : Pass {

			Array<glm::vec3> ssaoKernel = Array<glm::vec3>(64);

			GLuint fboNoBlur;

			asset::Shader *shaderBlur = nullptr;

			void init() override;
			void draw(GameState *) override;
		};

		struct PostFX : Pass {

			PostFX(std::string name) {

				this->name = name;
			}

			std::string name;
			size_t previous = 0;

			GLfloat test = 0.0;

			void init() override;
			void draw(GameState *) override;
		};
	}
}

#endif

