#ifndef SHADER_H
#define SHADER_H

namespace flare {

    namespace asset {

	// TODO: Find a better solution to storing all attrib and uniform locators
	struct Shader : Asset {

	    GLuint id = 0;

	    struct {

		GLuint position = 0;
		GLuint texture = 0;
		GLuint normal = 0;

		GLuint model = 0;
		GLuint view = 0;
		GLuint projection = 0;

		GLuint viewPosition = 0;
		
		struct {
		    GLuint shininess = 0;
		} material;

		struct {
		    GLuint position = 0;
		    GLuint ambient = 0;
		    GLuint diffuse = 0;
		    GLuint specular = 0;
		} light;

	    } locations;

	    void _load() override;
	};
    }
}

#endif

