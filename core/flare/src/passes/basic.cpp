#include "flare/flare.h"

void flare::render::passes::Basic::init() {

	shader = asset::load<asset::Shader>("core/basic");

	GLuint texture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	GLuint depthTexture = 0;

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D,depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	textures.add(texture);
}

void flare::render::passes::Basic::draw(GameState *gameState) {

	flare::render::setShader(shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	State::Render *render = &getState()->render;
	glUniform3fv(render->shader->locations.light.direction, 1, glm::value_ptr(render->light.direction));
	glUniform3fv(render->shader->locations.light.ambient, 1, glm::value_ptr(render->light.ambient));
	glUniform3fv(render->shader->locations.viewPosition, 1, glm::value_ptr(render->camera.position));

	getState()->render.pass = State::Render::GEOMETRY;
	gameState->manager->draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
