#ifndef PASSES_H
#define PASSES_H

namespace flare {

	class GameState;

	namespace render {
		namespace passes {

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

			struct Geometry : Pass {

				enum GBUFFER_TEXTURE_TYPE {
					GBUFFER_TEXTURE_TYPE_POSITION,
					GBUFFER_TEXTURE_TYPE_DIFFUSE,
					GBUFFER_TEXTURE_TYPE_NORMAL,
					GBUFFER_TEXTURE_TYPE_TEXCOORD,
					GBUFFER_TEXTURE_TYPE_DIFFUSE_LIGHT,
					GBUFFER_TEXTURE_TYPE_SPECULAR_LIGHT,
					GBUFFER_NUM_TEXTURES
				};

				/** @todo Is this really needed? */
				GLuint depthTexture = 0;

				void init() override;
				void draw(GameState *gameState) override;
			};

			struct Skybox : Pass {

				GLuint skyboxTexture = 0;
				GLuint vao;

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

			struct Lighting : Pass {

				void init() override;
				void draw(GameState *) override;
			};
		}
	}
}

#endif

