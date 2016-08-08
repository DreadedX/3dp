#include <sstream>

#include <GL/glew.h>

#include "flux/file.h"
#include "flux/read.h"
#include "flare/engine.h"
#include "flare/assets/shader.h"

/** @todo Add a glsl error check to the glsl plugin */
void printError(char* error, const char* source, std::string name) {

		print_w("Shader failed to compile (%s)", name.c_str());

		std::istringstream errorStream(error);
		std::string errorLine;

		while (std::getline(errorStream, errorLine)) {

			int lineNumberStart = errorLine.find("(");
			int lineNumberEnd = errorLine.find(")");

			std::string lineNumber = errorLine.substr(lineNumberStart + 1, lineNumberEnd - lineNumberStart - 1);

			std::istringstream sourceStream(source);
			std::string sourceLine;

			print_w(errorLine.c_str());

			int count = 1;
			while(std::getline(sourceStream, sourceLine)) {

				if (lineNumber.compare(std::to_string(count-1)) == 0 || lineNumber.compare(std::to_string(count+1)) == 0 ) {

					print_w("	%i: \033[37m%s\033[39m", count, sourceLine.c_str());
				}
				if (lineNumber.compare(std::to_string(count)) == 0) {

					print_w("	%i: \033[39m%s", count, sourceLine.c_str());
				}
				count++;
			}
		}
}

void deleteShader(flare::asset::Shader *shader) {

	if (shader->id != 0) {

		glDeleteProgram(shader->id);
		shader->id = 0;
	}
}

void flare::asset::Shader::_load() {

	// Shader setup stuff
	// This should be loaded from the shaders
	addLocation("model");
	addLocation("view");
	addLocation("projection");
	addLocation("depthMVP");
	addLocation("material.shininess");
	addLocation("light.direction");
	addLocation("light.ambient");
	addLocation("light.diffuse");
	addLocation("light.specular");
	addLocation("viewPosition");
	addLocation("toggle");

	// This should be loaded from the shaders
	addTexture("material.diffuse", 0);
	addTexture("material.normal", 1);
	addTexture("material.specular", 2);
	addTexture("render", 1);
	addTexture("shadow", 3);
	addTexture("gPositionMap", 0);
	addTexture("gColorMap", 1);
	addTexture("gNormalMap", 2);
	addTexture("gTexCoordMap", 3);
	addTexture("gDiffuseColorMap", 4);
	addTexture("gSpecularColorMap", 5);
	addTexture("ssaoBlurTexture", 6);
	addTexture("skyboxTexture", 7);
	addTexture("ssaoTexture", 0);
	addTexture("noiseTexture", 1);
	addTexture("skybox", 0);

	deleteShader(this);

	flux::File *shaderFile = flux::read::get(name);

	byte *shaderSource = shaderFile->load();

	uint offset = 0;
	size_t vertexLength = 0;
	size_t fragmentLength = 0;

	for (uint i = 0; i < sizeof(size_t); ++i) {
		vertexLength += shaderSource[offset + i] << (i*8);
	}
	offset += sizeof(size_t);
	for (uint i = 0; i < sizeof(size_t); ++i) {
		fragmentLength += shaderSource[offset + i] << (i*8);
	}
	offset += sizeof(size_t);

	char *vertexSource = allocator::make_new_array<char>(*getState()->proxyAllocators.flux, vertexLength);
	char *fragmentSource = allocator::make_new_array<char>(*getState()->proxyAllocators.flux, fragmentLength);

	for(uint i = 0; i < vertexLength; i++) {

		vertexSource[i] = shaderSource[offset + i];
	}
	offset += vertexLength;
	for(uint i = 0; i < fragmentLength; i++) {

		fragmentSource[i] = shaderSource[offset + i];
	}
	allocator::make_delete_array(*getState()->proxyAllocators.flux, shaderSource);

	// Load shader source
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	// Compile shader
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Check compile status
	GLint vertexStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexStatus);
	if (vertexStatus != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);

		printError(buffer, vertexSource, name + "/vertex");

		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, vertexSource);
		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, fragmentSource);
		return;
	}

	GLint fragmentStatus;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentStatus);
	if (fragmentStatus != GL_TRUE) {
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);

		printError(buffer, fragmentSource, name + "/fragment");

		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, vertexSource);
		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, fragmentSource);
		return;
	}

	allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, vertexSource);
	allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, fragmentSource);

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
		print_w("Shader link error: (%s)\n%s", name.c_str(), buffer);
	}
	
	// Set locations for the shader
	for (auto i : locations) {

		locations[i.first] = glGetUniformLocation(id, i.first.c_str());
	}
}

void flare::asset::Shader::addLocation(const char *locationName) {

		locations[locationName] = glGetUniformLocation(id, locationName);
}

int flare::asset::Shader::getLocation(const char *locationName) {

	assert(locations.find(locationName) != locations.end());

	return locations[locationName];
}

void flare::asset::Shader::addTexture(const char* textureName, int textureUnit) {

	textures[textureName] = textureUnit;
	addLocation(textureName);
}

void flare::asset::Shader::use() {

	State::Render *render = &getState()->render;

    if (render->shader == nullptr || render->shader->id != id) {

		glUseProgram(id);

		for(auto i : textures) {

			int location = glGetUniformLocation(id, i.first.c_str());
			glUniform1i(location, i.second);
		}

		render->shader = this;
    }
}


/* @todo Is this really needed */
int flare::asset::Shader::getTexture(const char *textureName) {

	assert(textures.find(textureName) != textures.end());

	return textures[textureName];
}

flare::asset::Shader::~Shader() {

	deleteShader(this);
}
