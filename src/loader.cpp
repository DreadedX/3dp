#include "flare/flare.h"

// TODO: Make this load from a external file, preferably a .cpp or .h file
char *test_vsh = {"#version 150 core\nin vec2 position;\nvoid main() {\ngl_Position = vec4(position, 0.0, 1.0);\n}\n"};
char *test_fsh = {"#version 150 core\nout vec4 outColor;\nvoid main() {\noutColor = vec4(1.0, 1.0, 1.0, 1.0);\n}\n"};

// TODO: Make this function accept argument in order to load other shaders
GLuint flare::loadShader() {

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
	printf("Vertex shader error: %s\n", buffer);
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
	printf("Fragment shader error: %s\n", buffer);
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
	printf("Shader link error: %s\n", buffer);
    }

    return shaderProgram;
}
