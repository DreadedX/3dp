#include "flare/flare.h"

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {

	return a + f * (b - a);
}

void generateSSAOKernel(flare::render::passes::SSAO *ssao) {
	
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (GLuint i = 0; i < 64; i++) {

		glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		GLfloat scale = GLfloat(i) / 64.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssao->ssaoKernel.add(sample);
	}

	Array<glm::vec3> ssaoNoise = Array<glm::vec3>(16);
	for (GLuint i = 0; i < 16; i++) {

		glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f
			);
		ssaoNoise.add(noise);
	}

	GLuint textureNoise = 0;

	glGenTextures(1, &textureNoise);
	glBindTexture(GL_TEXTURE_2D, textureNoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &textureNoise);

	ssao->textures.add(textureNoise);
}

void flare::render::passes::SSAO::init() {

	shader = asset::load<asset::Shader>("base/ssao");
	shaderBlur = asset::load<asset::Shader>("base/ssaoBlur");

	glGenFramebuffers(1, &fboNoBlur);  
	glBindFramebuffer(GL_FRAMEBUFFER, fboNoBlur);

	GLuint textureNoBlur = 0;

	glGenTextures(1, &textureNoBlur);
	glBindTexture(GL_TEXTURE_2D, textureNoBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNoBlur, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	textures.add(textureNoBlur);

	GLuint textureBlur = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &textureBlur);
	glBindTexture(GL_TEXTURE_2D, textureBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getSettings()->resolution.x, getSettings()->resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBlur, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	textures.add(textureBlur);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	generateSSAOKernel(this);
}

void flare::render::passes::SSAO::draw() {

	// SSAO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboNoBlur);

    glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(shader);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, getState()->renderPasses[0]->textures[Geometry::GBUFFER_TEXTURE_TYPE_POSITION]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, getState()->renderPasses[0]->textures[Geometry::GBUFFER_TEXTURE_TYPE_NORMAL]);

	for (GLuint i = 0; i < 64; ++i) {

		glUniform3fv(glGetUniformLocation(getState()->shader->id, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
	}

	glUniformMatrix4fv(getState()->shader->locations.view, 1, GL_FALSE, glm::value_ptr(getState()->view));
	glUniformMatrix4fv(getState()->shader->locations.projection, 1, GL_FALSE, glm::value_ptr(getState()->projection));

	for (flare::asset::model::Mesh *mesh : getState()->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	// Blur
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glClear(GL_COLOR_BUFFER_BIT);

	flare::render::setShader(shaderBlur);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

	for (flare::asset::model::Mesh *mesh : getState()->quad->meshes) {

		glBindVertexArray(mesh->vao);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
