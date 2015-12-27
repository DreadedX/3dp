#include "flare/flare.h"

void flare::asset::Shader::_load() {

    if (id != 0) {

	glDeleteProgram(id);
	id = 0;
    }
    
    flux::FileLoad *shaderConfigFile = flux::get(name);
    const char *shaderConfig = reinterpret_cast<const char*>(shaderConfigFile->get(true));

    jsoncons::json configJson = jsoncons::json::parse_string(shaderConfig);
    delete[] shaderConfig;

    // TODO: Shader never check if they have been updated
    flux::FileLoad *vertexFile = flux::get(configJson.get("vertex", "Unknown Vertex Shader").as<std::string>());
    flux::FileLoad *fragmentFile = flux::get(configJson.get("fragment", "Unknown Fragment Shader").as<std::string>());

    const char *vertexSource = reinterpret_cast<const char*>(vertexFile->get(true));
    const char *fragmentSource = reinterpret_cast<const char*>(fragmentFile->get(true));

    // Load shader source
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    // Compile shader
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    delete[] vertexSource;
    delete[] fragmentSource;

    // Check compile status
    GLint vertexStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
    if (vertexStatus != GL_TRUE) {
	char buffer[512];
	glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
	print::w("Vertex shader error: %s", buffer);
	return;
    }

    GLint fragmentStatus;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
    if (fragmentStatus != GL_TRUE) {
	char buffer[512];
	glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
	print::w("Fragment shader error: %s", buffer);
	return;
    }

    // Combine shaders
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    // Check program status
    GLint programStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &programStatus);
    if (programStatus != GL_TRUE) {
	char buffer[512];
	glGetProgramInfoLog(id, 512, NULL, buffer);
	print::w("Shader link error: %s", buffer);
    }

    // Shader setup stuff
    locations.model = glGetUniformLocation(id, "model");
    locations.view = glGetUniformLocation(id, "view");
    locations.projection = glGetUniformLocation(id, "projection");

    locations.material.shininess = glGetUniformLocation(id, "material.shininess");

    locations.light.position = glGetUniformLocation(id, "light.position");
    locations.light.ambient = glGetUniformLocation(id, "light.ambient");
    locations.light.diffuse = glGetUniformLocation(id, "light.diffuse");
    locations.light.specular = glGetUniformLocation(id, "light.specular");

    locations.viewPosition = glGetUniformLocation(id, "viewPosition");
}
