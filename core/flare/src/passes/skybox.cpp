#include "flare/flare.h"

GLfloat skyboxVertices[] = {
    // Positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void flare::render::passes::Skybox::init() {

	shader = asset::load<asset::Shader>("core/skybox");

	// Load the skybox
	/** @todo This needs to be an asset */
	
	Array<std::string> faces;

	faces.add("core/right");
	faces.add("core/left");
	faces.add("core/top");
	faces.add("core/bottom");
	faces.add("core/front");
	faces.add("core/back");

	glGenTextures(1, &skyboxTexture);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	for (GLuint i = 0; i < faces.size(); ++i) {

		flux::FileLoad *textureFile = flux::get(faces[i]);
		byte *textureData = textureFile->get();

		int width = 0;
		int height = 0;

		uint offset = 0;
		for (uint i = 0; i < sizeof(int); ++i) {
			width += textureData[i] << (i*8);
		}
		offset += sizeof(int);

		for (uint i = 0; i < sizeof(int); ++i) {
			height += textureData[i + offset] << (i*8);
		}
		offset += sizeof(int);

		// Compenstate for bytes per pixel thing
		offset += sizeof(byte);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData + offset);

		// delete[] textureData;
		allocator::make_delete_array<byte>(*getState()->proxyAllocators.flux, textureData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	/////////////////////////////////////////////
	GLuint skyboxRenderedTexture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &skyboxRenderedTexture);
	glBindTexture(GL_TEXTURE_2D, skyboxRenderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, skyboxRenderedTexture, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	textures.add(skyboxRenderedTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint vbo = 0;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*3, (GLvoid*)0);
	glBindVertexArray(0);
}

void flare::render::passes::Skybox::draw(GameState *gameState) {

	State::Render *render = &getState()->render;
	
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glDepthMask(GL_FALSE);
	glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glActiveTexture(GL_TEXTURE1);
	/** @todo Make this not hardcoded and use the previous shader step */
	glBindTexture(GL_TEXTURE_2D, gameState->renderPasses[2]->textures[1]);

	glm::mat4 view = glm::mat4(glm::mat3(render->view));
	glUniformMatrix4fv(render->shader->locations.view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(render->shader->locations.projection, 1, GL_FALSE, glm::value_ptr(render->projection));

	for (flare::asset::model::Mesh *mesh : render->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indexSize, GL_UNSIGNED_INT, 0);
	}

	// glBindVertexArray(vao);
	// glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_TRUE);
}
