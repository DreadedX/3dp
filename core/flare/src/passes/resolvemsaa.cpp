#include <GL/glew.h>

#include "glm/gtc/type_ptr.hpp"

#include "flare/engine.h"
#include "flare/passes.h"

void flare::render::passes::ResolveMSAA::init() {

	shader = asset::load<flare::asset::Shader>("core/shader/resolvemsaa");

	GLuint texture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print_e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(texture);
}

void flare::render::passes::ResolveMSAA::draw(GameState *) {

	State::Render *render = &getState()->render;
	
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// This one only works when we acctually load all the info from the shader somehow
	// render::setTexture(shader->getTexture("render"), getShaderOutput("render"));
	// render::setTexture(0, getShaderOutput("render"));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, getShaderOutput("render"));
	

	glClear(GL_COLOR_BUFFER_BIT);

	shader->use();

		{
			glUniform3fv(shader->locations["light.direction"], 1, glm::value_ptr(render->light.direction));
			glUniform3fv(shader->locations["light.ambient"], 1, glm::value_ptr(render->light.ambient));

			glUniform3fv(shader->locations["viewPosition"], 1, glm::value_ptr(render->camera.position));
		}

	glUniformMatrix4fv(shader->locations["model"], 1, GL_FALSE, glm::value_ptr(glm::mat4()));

	render::quad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	setShaderOutput("render", textures[0]);
}
