#include "flare/flare.h"

std::vector<flare::asset::Asset*> flare::asset::assetList;

void flare::asset::reload() {
    
    flux::reload();

    for (Asset *asset : assetList) {

	print::d("Reloading asset: %s", asset->name.c_str());
	asset->_load();
    }
}

void flare::asset::ShaderAsset::_load() {

    // TODO: Shader never check if they have been updated
    flux::FileLoad *vertexFile = flux::get(name + "_vertex");
    flux::FileLoad *fragmentFile = flux::get(name + "_fragment");

    const char *vertexSource = reinterpret_cast<const char*>(vertexFile->get(true));
    const char *fragmentSource = reinterpret_cast<const char*>(fragmentFile->get(true));

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
	print::w("Vertex shader error: %s", buffer);
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
	print::w("Fragment shader error: %s", buffer);
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

    delete[] vertexSource;
    delete[] fragmentSource;
}

void flare::asset::TextureAsset::_load() {

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glGenerateMipmap(GL_TEXTURE_2D);
    flux::FileLoad *textureFile = flux::get(name);
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

}
