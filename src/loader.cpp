#include "flare/flare.h"

// TODO: Make this load from a external file, preferably a .cpp or .h file
const char *test_vsh = {"#version 150 core\nin vec2 position;\nin vec3 color;\nin vec2 texcoord;\nout vec3 Color;\nout vec2 Texcoord;\nvoid main() {\nColor = color;\nTexcoord = texcoord;\ngl_Position = vec4(position, 0.0, 1.0);\n}\n"};
const char *test_fsh = {"#version 150 core\nin vec3 Color;\nin vec2 Texcoord;\nout vec4 outColor;\nuniform sampler2D tex;\nvoid main() {\noutColor = texture(tex, Texcoord) * vec4(Color, 1.0);\n}\n"};

// TODO: Make this function accept argument in order to load other shaders
GLuint flare::shader::load() {

    // Load vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &test_vsh, NULL);

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
    glShaderSource(fragmentShader, 1, &test_fsh, NULL);

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

    return shaderProgram;
}

GLuint flare::texture::load() {

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glGenerateMipmap(GL_TEXTURE_2D);

    float pixels[] = {
	0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

    return texture;
}
