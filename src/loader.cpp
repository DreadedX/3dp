#include "flare/flare.h"

GLuint flare::shader::load(std::string name) {

    // TODO: Shader never check if they have been updated
    flux::File *vertexFile = flux::get(name + "_vertex");
    vertexFile->inUse = true;
    flux::File *fragmentFile = flux::get(name + "_fragment");
    fragmentFile->inUse = true;

    const char *vertexSource = reinterpret_cast<const char*>(vertexFile->get());
    const char *fragmentSource = reinterpret_cast<const char*>(fragmentFile->get());

    // Load vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);

    // Compile vertex shader
    glCompileShader(vertexShader);

    // Check compile status
    GLint vertexStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
    if (vertexShader != GL_TRUE) {
	char buffer[512];
	glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
	log::w("Vertex shader error: %s", buffer);
    }
    
    // Load fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    // Compile fragment shader
    glCompileShader(fragmentShader);

    // Check compile status
    GLint fragmentStatus;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
    if (fragmentStatus != GL_TRUE) {
	char buffer[512];
	glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
	log::w("Fragment shader error: %s", buffer);
    }

    // Combine shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check program status
    GLint programStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programStatus);
    if (programStatus != GL_TRUE) {
	char buffer[512];
	glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
	log::w("Shader link error: %s", buffer);
    }

    delete[] vertexSource;
    delete[] fragmentSource;

    return shaderProgram;
}

GLuint flare::texture::load(flare::flux::File *textureFile) {

    // TODO: What happens when multiple texture reload and free the same thing
    static std::map<flux::File*, GLuint> map;
    if (textureFile->inUse && map.find(textureFile) != map.end()) {

	log::d("Texture '%s' already loaded", textureFile->name.c_str());
	return map[textureFile];
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glGenerateMipmap(GL_TEXTURE_2D);

    textureFile->inUse = true;
    byte *pixels = textureFile->get();

    int width = 0;
    int height = 0;
    if (textureFile->extraSize >= 9) {
	for (int i = 0; i < 4; ++i) {
	    width += textureFile->extra[i] << (i*8);
	}
	for (int i = 0; i < 4; ++i) {
	    height += textureFile->extra[i+4] << (i*8);
	}
    }

    // TODO: Check bytes per pixel
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    delete[] pixels;

    map[textureFile] = texture;

    return texture;
}
