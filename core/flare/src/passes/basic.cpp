#include "flare/flare.h"

void flare::render::passes::Basic::init() {

	shader = asset::load<asset::Shader>("core/shader/basic");

	GLuint texture = 0;

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	textures.add(texture);
		
	for (int i = 0; i < 2; i++) {

		GLuint extraTexture = 0;

		glGenTextures(1, &extraTexture);
		glBindTexture(GL_TEXTURE_2D, extraTexture);
		if (i == 0) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 + i, GL_TEXTURE_2D, extraTexture, 0);

		textures.add(extraTexture);
	}

	GLuint depthTexture = 0;

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, getState()->settings.resolution.x, getState()->settings.resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
	glDrawBuffers(3, drawBuffers);

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		print::e("FB error, status: 0x%x", status);
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			print::e("error");
		}
		exit(-1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	glUniformMatrix4fv(render->shader->locations.view, 1, GL_FALSE, glm::value_ptr(render->view));
	glUniformMatrix4fv(render->shader->locations.projection, 1, GL_FALSE, glm::value_ptr(render->projection));

	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-400, 400, -400, 400, -400, 400);
	glm::mat4 depthViewMatrix = glm::lookAt(-render->light.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 depthModelMatrix = glm::mat4(1.0f);

	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);
	glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

	glUniformMatrix4fv(render->shader->locations.depthMVP, 1, GL_FALSE, &depthBiasMVP[0][0]);

	glActiveTexture(GL_TEXTURE3);
	/** @todo Make this not hardcoded and use the previous shader step */
	glBindTexture(GL_TEXTURE_2D, gameState->renderPasses[0]->textures[0]);

	getState()->render.pass = State::Render::GEOMETRY;
	gameState->manager->draw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
