#include <random>
#include <GL/glew.h>

#include "glm/gtc/type_ptr.hpp"

#include "flare/engine.h"

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {

	return a + f * (b - a);
}

void generateSSAOKernel(flare::render::passes::SSAO *ssao) {
	
	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
	std::default_random_engine generator;
	for (GLuint i = 0; i < 128; i++) {

		glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);

		GLfloat scale = GLfloat(i) / 128.0;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssao->ssaoKernel.add(sample);
	}

	Array<glm::vec3> ssaoNoise = Array<glm::vec3>(4);
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

	shader = asset::load<asset::Shader>("core/shader/ssao");
	shaderBlur = asset::load<asset::Shader>("core/shader/ssaoBlur");

	glGenFramebuffers(1, &fboNoBlur);  
	glBindFramebuffer(GL_FRAMEBUFFER, fboNoBlur);

	GLuint textureNoBlur = 0;

	glGenTextures(1, &textureNoBlur);
	glBindTexture(GL_TEXTURE_2D, textureNoBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureNoBlur, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print_e("FB error, status: 0x%x", status);
		exit(-1);
	}

	textures.add(textureNoBlur);

	GLuint textureBlur = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &textureBlur);
	glBindTexture(GL_TEXTURE_2D, textureBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBlur, 0);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print_e("FB error, status: 0x%x", status);
		exit(-1);
	}

	textures.add(textureBlur);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	generateSSAOKernel(this);
}

void flare::render::passes::SSAO::draw(GameState *) {

	State::Render *render = &getState()->render;

	// SSAO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboNoBlur);

    glClear(GL_COLOR_BUFFER_BIT);

	shader->use();

	render::setTexture(shader->getTexture("gPositionMap"), getShaderOutput("gPositionMap"));
	render::setTexture(shader->getTexture("noiseTexture"), textures[2]);
	render::setTexture(shader->getTexture("gNormalMap"), getShaderOutput("gNormalMap"));

	for (GLuint i = 0; i < 128; ++i) {

		glUniform3fv(glGetUniformLocation(render->shader->id, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
	}

	glUniformMatrix4fv(render->shader->getLocation("view"), 1, GL_FALSE, glm::value_ptr(render->view));
	glUniformMatrix4fv(render->shader->getLocation("projection"), 1, GL_FALSE, glm::value_ptr(render->projection));

	render::quad();

	// Blur
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    glClear(GL_COLOR_BUFFER_BIT);

	shaderBlur->use();

	render::setTexture(shader->getTexture("ssaoTexture"), textures[0]);
	render::setTexture(shader->getTexture("render"), getShaderOutput("render"));

	render::quad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	setShaderOutput("render", textures[1]);
}
