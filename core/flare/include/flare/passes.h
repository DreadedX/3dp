#ifndef PASSES_H
#define PASSES_H

namespace flare {
	namespace render {
		namespace passes {

			struct Pass {

				GLuint fbo;
				GLuint *textures = nullptr;

				asset::Shader *shader = nullptr;

				virtual void init() {}
				virtual void draw() {}

				virtual ~Pass() {

					if (textures != nullptr) {

						delete[] textures;
					}
				}
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
				void draw() override;
			};

			struct SSAO : Pass {

				std::vector<glm::vec3> ssaoKernel;

				GLuint fboNoBlur;

				asset::Shader *shaderBlur = nullptr;

				void init() override;
				void draw() override;
			};

			struct Lighting : Pass {

				void init() override;
				void draw() override;
			};
		}
	}
}

#endif

