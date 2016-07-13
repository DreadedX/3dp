#ifndef SHADER_H
#define SHADER_H

namespace flare {

	namespace asset {

		/** @brief Asset struct containing shader info
			@todo Find a better solution to storing all attrib and uniform locators (Maybe an array and using an enum to get the correct value) */
		struct Shader : Asset {

			/** @brief Shader id */
			GLuint id = 0;

			~Shader();

			struct {

				// GLuint position = 0;
				// GLuint texture = 0;
				// GLuint normal = 0;

				/** @brief Uniform location of model matrix */
				GLuint model = 0;
				/** @brief Uniform location of view matrix */
				GLuint view = 0;
				/** @brief Uniform location of projection matrix */
				GLuint projection = 0;
				/** @brief Uniform location of projection matrix */
				GLuint depthMVP = 0;

				/** @brief Uniform location of view position */
				GLuint viewPosition = 0;

				struct {
					/** @brief Material shininess
						@todo This should propably use Material Asset (?) */
					GLuint shininess = 0;
				} material;

				struct {
					/** @brief Uniform location of light direction vector */
					GLuint direction = 0;
					/** @brief Uniform location of ambient light color */
					GLuint ambient = 0;
					/** @brief Uniform location of diffuse light color */
					GLuint diffuse = 0;
					/** @brief Uniform location of specular light color */
					GLuint specular = 0;
				} light;

				/** @brief Uniform location of a toggle that can be used in the shader for testing */
				GLuint toggle = 0;
			} locations;

			/** @brief Load shader */
			void _load() override;
		};
	}
}

#endif

