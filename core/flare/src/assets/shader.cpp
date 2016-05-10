#include "flare/flare.h"

#include <sstream>

/** @todo Figure out a way to map this to the special glsl files, an idea is to add a shader compiler to the glsl plugin and map it back to the original file */
void printError(char* error, const char* source, std::string name) {

		print::w("Shader failed to compile (%s)", name.c_str());

		std::istringstream errorStream(error);
		std::string errorLine;

		while (std::getline(errorStream, errorLine)) {

			int lineNumberStart = errorLine.find("(");
			int lineNumberEnd = errorLine.find(")");

			std::string lineNumber = errorLine.substr(lineNumberStart + 1, lineNumberEnd - lineNumberStart - 1);

			std::istringstream sourceStream(source);
			std::string sourceLine;

			print::w(errorLine.c_str());

			int count = 1;
			while(std::getline(sourceStream, sourceLine)) {

				if (lineNumber.compare(std::to_string(count-1)) == 0 || lineNumber.compare(std::to_string(count+1)) == 0 ) {

					print::w("	%i: \033[37m%s\033[39m", count, sourceLine.c_str());
				}
				if (lineNumber.compare(std::to_string(count)) == 0) {

					print::w("	%i: \033[39m%s", count, sourceLine.c_str());
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

	deleteShader(this);

	flux::FileLoad *vertexFile = flux::get(name + "/vertex");
	flux::FileLoad *fragmentFile = flux::get(name + "/fragment");

	char *vertexSource = reinterpret_cast<char*>(vertexFile->get(true));
	char *fragmentSource = reinterpret_cast<char*>(fragmentFile->get(true));

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

		// delete[] vertexSource;
		// delete[] fragmentSource;
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

		// delete[] vertexSource;
		// delete[] fragmentSource;
		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, vertexSource);
		allocator::make_delete_array<char>(*getState()->proxyAllocators.flux, fragmentSource);
		return;
	}

	// delete[] vertexSource;
	// delete[] fragmentSource;
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
		print::w("Shader link error: (%s)\n%s", name.c_str(), buffer);
	}

	// Shader setup stuff
	locations.model = glGetUniformLocation(id, "model");
	locations.view = glGetUniformLocation(id, "view");
	locations.projection = glGetUniformLocation(id, "projection");

	locations.material.shininess = glGetUniformLocation(id, "material.shininess");

	locations.light.direction = glGetUniformLocation(id, "light.direction");
	locations.light.ambient = glGetUniformLocation(id, "light.ambient");
	locations.light.diffuse = glGetUniformLocation(id, "light.diffuse");
	locations.light.specular = glGetUniformLocation(id, "light.specular");

	locations.viewPosition = glGetUniformLocation(id, "viewPosition");

	locations.toggle = glGetUniformLocation(id, "toggle");
}

flare::asset::Shader::~Shader() {

	deleteShader(this);
}
